# demo-jvm 架构与设计决策

一个从零手写的 C++20 JVM 解释器。本文记录**关键设计决策及其理由(why,而非只是 what)**,作为长期参考。

---

## 模块划分

依赖单向:`engine → runtime → oops → utilities`;`classfile → oops → utilities`;`memory → oops → utilities`。

| 模块 | 职责 |
|---|---|
| `utilities` | 叶子原语:`types`(U1/U2/Jint…)、`Slot`、`descriptor` 解析器、`access_flags`、`endian`。无依赖。 |
| `oops` | 运行期元数据:`Klass`/`Method`/`Field`/`RuntimeConstantPool`/`MethodArea`/`Object`/`StringPool`(仿 HotSpot "ordinary object pointers")。 |
| `classfile` | `.class` 解析:`ByteReader`、`ClassFileParser`、`ClassFile`、原始 `ConstantPool`、`ClassLoader`。 |
| `runtime` | 执行状态:`Frame`、`OperandStack`、`LocalVariables`、`Thread`、`Stack`。 |
| `engine` | 解释器:字节码分派、`NativeRegistry`、符号拦截。 |
| `memory` | 堆:无 GC 的 bump arena。 |

**命名规约:目录名 = 命名空间叶子 = CMake target(`jvm_<name>`)= 测试目录**。`utilities` 额外承载 `jvm::descriptor`。

---

## Slot:64 位、无 tag 的裸 union

**决策**:Slot 是固定 64 位 union `{ Jint i; Jfloat f; Jlong l; Jdouble d; Jref r; }`,**不带类型 tag**。

**理由**
- **64 位(机器字)而非 32 位**:64 位主机上引用(`void*`)8 字节,必须一格放下;32 位槽会立刻逼出句柄/压缩 oop。机器字槽正是 HotSpot 在 LP64 的做法;规范里"32 位 slot"是计数/校验层抽象,不是存储宽度强制。
- **无 tag**:tag 冗余——(已校验的)opcode 已经告诉你类型,槽不必自述。tag 每槽多耗 8 字节(对齐)且需纪律维持一致。**类型信息存在 API(typed push/pop)和 opcode 里,不在数据里**。(需要开发期安全网可在 `NDEBUG` 下加回 debug-only tag。)

**category-2(long/double)处理**
- 值一格放得下,**不拆高低字**(拆是 32 位技术)。
- **操作数栈**:category-2 占**两格**(值 + 一个无 tag 占位)。这是必须的——DUP2/POP2 等按 form 定义而 opcode 不编码 form,无类型信息下靠两格占位统一处理。`pushWide`/`popWide` 维护它。
- **局部变量表**:值存在索引 `n`,`n+1` 是**保留但不用的死格**(不写占位)。这里"两格"仅是计数/索引规则。
- **后果**:category-2 跨表示搬运(操作数栈 ↔ 局部表)必须走 typed 方法(`setWide`/`popWide`),**绝不裸槽拷贝**——这是曾经的 A2 bug 根源。

**tag vs oopmap(GC)**:推迟。当前直接指针 + 无 GC;将来 GC 用 oopmap(或运行期 ref 追踪)做精确根,而非复活 per-slot tag。

---

## PC:归 Frame

**决策**:程序计数器存在 `Frame::pc_`,**不在 Thread**。解释器执行期把它缓存进一个本地寄存器,在帧边界 spill/reload。

**理由**
- 嵌套调用本就需要每个挂起帧存一份 pc(返回地址);单个线程级 pc 不够用。
- per-frame pc 把"活 pc(栈顶帧)"和"保存 pc(调用者)"统一成一个字段。invoke = 把本地 pc spill 进调用者帧、压 callee(pc=0);return = 弹帧、从新栈顶帧 reload 本地 pc。
- **合规**:规范"每线程一个 pc 寄存器"约束的是**行为/隔离**(各线程位置私有、单一活 pc),不是存储位置。栈顶帧的 pc **就是**线程的活 pc。HotSpot 也是每解释帧存 bcp + 一个寄存器,与此一致。
- **多线程友好**:pc 随各线程的栈走,无共享 pc;绿色线程切换 = spill → 换栈 → reload。

**纪律**:本地 pc 是帧执行期的权威;**任何栈操作(invoke/return/throw)前先 spill 回 `Frame::pc_`**,让观察者(及将来的异常展开)看到正确 pc。

---

## 引用与堆(无 GC)

**决策**:`Jref = void*`(typedef),一个指进**无 GC bump arena** 的**直接指针**。对象永不移动、永不回收。

**理由**
- 无 GC + 非移动 → 直接指针既正确又最简。句柄只被**移动式** GC 逼出;非移动的 mark-sweep 也保持直接指针,故不付句柄间接税。
- `Jref` 保持 typedef、解引用走小 API,故日后若选移动式收集器,表示可换(句柄/压缩 oop)——推迟。

**堆**:bump 指针 arena(`memory/heap.h`),大 byte 缓冲 + top 偏移,8 字节对齐,OOM 抛异常。Meyer's 单例,**编译期固定 64MB**(不可运行期配置——刻意的限制;要 `-Xmx` 改成启动期 init)。

**对象布局**(单块连续分配):
```
+0   Klass*    klass    (类型:字段布局 / 分派 / instanceof)
+8   MarkWord  mark     (预留:GC mark/forwarding、hash、锁 —— 当前不透明)
+16  Slot      fields[instance_slot_count]   (内联,零初始化 = Java 默认值)
```
- **无虚函数**(保证 `this+1` = 头末尾)。内联字段用 `reinterpret_cast<Slot*>(this + 1)`——标准 C++、`-Wpedantic` 干净(优于非标准的柔性数组成员)。placement-new 头部;字段靠 arena 的零当默认值。
- **MarkWord**:现在**预留**这个字(HotSpot 式两字头),日后**不重排布局**即可长成位复用的 mark word。**不要现在实现 markOop 的位编码**(它复用 hash/age/锁/forwarding——都是我们还没有的特性)。

**字段继承布局**:`instance_slot_count` **从父类总槽数起算**(`prepareFieldsAndStatics` 里 `super->getInstanceSlotCount()` 作为子类字段起点),父类字段在前、子类字段接续——继承字段读写正确、字段隐藏成立。静态字段各类自有 `statics_`(不继承布局)。

---

## Klass-oop 层级(仿 HotSpot)

**决策**:两棵平行的树——**Klass(元数据,MethodArea 持有)** 与 **oop(堆对象头)** 各自成层级、一一对应。

```
Klass (抽象基:kind/state/name/super + 虚 isInstanceOf/getClassLoader/getDescriptorName)
├── InstanceKlass          (.class 加载:methods/fields/CP/statics)
└── ArrayKlass (中间基:element_size_)
    ├── TypeArrayKlass      (基元数组 [I/[J/…,带 BasicType)
    └── ObjArrayKlass       (引用数组 [L…;/[[…,带 element_klass_)

OopDesc (头:klass + mark,无 vtable)
├── InstanceOopDesc         (内联 Slot 字段)
└── ArrayOopDesc            (+ length + 内联元素)
```

**理由 / 关键取舍**
- **Klass 侧用 C++ 虚函数**:元数据、实例少、非布局敏感。HotSpot 手搓 vtable 是为 metaspace/CDS 跨进程序列化,那些理由在本项目不成立,直接用虚函数换干净层级。基类有虚析构(`MethodArea` 以 `unique_ptr` 持派生对象)。
- **oop 侧零虚函数**:`OopDesc` 头必须保持 `klass + mark` 紧凑(`this+1` = 头末尾),一旦加 vptr 就破坏布局。**实例行为的多态全部经由 Klass 分派**(`oop->getKlass()->…`),类型判别走 `kind()`。这与 HotSpot 一致(`oopDesc` 无 vtable)。
- 数组 klass:`super` 恒为 `java/lang/Object`(当前 Object 被桩 → `super_class_` 留 `nullptr`,`isInstanceOf` 按名认 Object);构造即 `FullyInitialized`(数组无 `<clinit>`,故创建指令不做 init 检查)。

---

## 数组(D4):klass plumbing + 分阶段

数组是堆对象但布局不同(头 + `length` + 内联元素),且每个数组类型是一个**合成 klass**(无 `.class`)。落地分 5 阶段(路线见 `TODO.md`)。

**Phase A(已完成)——klass plumbing**
- **名字即描述符**:数组 klass 的 `name_` 就是其 JVM 描述符(`[I`、`[Ljava/lang/String;`、`[[I`),与 `CONSTANT_Class` 里的串一致 → 创建与 CHECKCAST 命中**同一单例**。
- **BasicType**(`utilities/basic_type.hpp`):atype 值(4..11)+ 名/尺寸/`char→BasicType` 表。位于 utilities 因 oops 与 engine 都要用。
- **工厂 + 去重**:`MethodArea` 加平行表 `array_klasses_`(name→`unique_ptr<ArrayKlass>`)+ `getOrCreateTypeArrayKlass(BasicType)` / `getOrCreateObjArrayKlass(Klass* component)`,**按名去重返回单例**(去重是正确性要求:`isInstanceOf` 用 `this==target` 指针相等)。`reset()` 两表都清。
- **`Klass::getDescriptorName()`**:Instance→`Lname;`,数组→`name_`(即描述符)。`ObjArrayKlass` 名 = `[` + `component->getDescriptorName()`。
- **`resolveClass` 收窄 + 路由**:变体 `InstanceKlass*→Klass*`,`resolveClass` 返回 `Klass*`;`[`-前缀名走 file-local `arrayKlassForName`(递归:`L…;` 走 `loadClass`,基元走 `getOrCreateTypeArrayKlass`,基底 1 维再包 (dim-1) 次)。路由放在 `constant_pool.cpp` 因那里已有 loader 访问,**不新增跨层依赖**。`resolveField/Method` 对目标 `static_cast<InstanceKlass*>`(成员引用恒为 instance)。
- **红利**:CHECKCAST/INSTANCEOF 对数组类型**自动可用**(`resolveClass` 产出数组 klass + `TypeArrayKlass`/`ObjArrayKlass::isInstanceOf`)。

**Phase B–E(待做,要点)**
- **B 堆分配**:`Heap::newTypeArray`/`newObjArray`;元素**按大小 packed**(与 `element_size_`/`base()` 骨架一致)。**对齐骨架已完成**:`ArrayOopDesc` 布局 `[ OopDesc header(16) | Jint length(4) | alignment padding(4) | element[N] ]`,`base()` = 偏移 24(8 对齐,long[]/double[] 不错位)。arena 零初始化且不复用 → 新数组自带零值(null/0)。仍在实现:实际堆分配函数与 NEWARRAY/ANEWARRAY/ARRAYLENGTH 指令。
- **C 创建/长度**:NEWARRAY(atype→BasicType)、ANEWARRAY(`resolveClass(component)` + `getOrCreateObjArrayKlass`)、ARRAYLENGTH;MULTIANEWARRAY 延后。
- **D 访问**:`*ALOAD`/`*ASTORE`,抽 null+越界 helper;B/C/S 符号/零扩展 + 存时截断;L/D 走 wide;AASTORE store-check 延后。
- **E 收尾**:真 `String[] args`;INVOKEVIRTUAL/INTERFACE 的 receiver klass cast 对数组加保护;`ObjArrayKlass::isInstanceOf` 递归元素协变(当前仅精确匹配);GC 期 `scanRefs`(ObjArray 扫元素、TypeArray no-op)。
- **错误路径**先 `std::runtime_error`(NegativeArraySize/NPE/AIOOBE),D6 后升级真异常。

---

## descriptor 与方法/字段签名

- 解析器(`utilities/descriptor.h`):`TypeKind { Int, Long, Float, Double, Ref, Void }`、`MethodType { params, ret, arg_slot_count }`,link 期解析一次,边界安全。
- `Method`/`Field` 存**原始 descriptor 字符串做身份**(`findMethod`/`findField` 比字符串——重载解析无损)**+** 缓存的 `MethodType`/`TypeKind` 供执行用(arity、槽数、宽度分派)。两个目的都需要:有损的解析形式区分不了重载(`f(String)` 与 `f(Object)` 都 → `Ref`),所以**不能拿它当身份键**。

---

## 常量池:自给自足的运行时 CP(C4)

**决策**:`RuntimeConstantPool` 在 link 期把符号引用**烘焙成字符串**;运行期解析**不再碰 ClassFile**。

- `SymRef_Class { class_name(点形式) }`;`SymRef_Field/Method { class_cp_index, member_name, descriptor }`。
- `prepareRuntimeConstantPool` 一次性把原始 CP 的 u2 索引解引用成字符串。`resolveClass/Field/Method` 用烘焙字符串(+ 运行时 cp 的 `class_cp_index` 导航)。`resolveNameAndType` 已删。
- **后果**:`Klass` 不再存 ClassFile 指针(Option B)。loader 把 `ClassFile*` 直接 thread 进 `prepare*`/`link*`(瞬时、构建期)。`MethodArea` 拥有 ClassFile。**oops 运行期对 classfile 零依赖**。

---

## 类加载

- `ClassLoader`(classfile)读取+解析+链接;`MethodArea`(oops 单例)按 `(loader, name)` 拥有 `(Klass, ClassFile)` 对。
- **名字形式(单一规范:斜杠)**:内部一律用**斜杠内部形式**(`java/lang/String`)——class 文件原生、与 descriptor 一致、同 HotSpot 内部 Symbol。`loadClass` 入口把输入 `.`→`/` **归一化一次**,此后 cache key / MethodArea key / `Klass::name_` / 路径 / SymRef_Class.class_name 全是斜杠且相等。**点形式只在人机/API 边界**(CLI main-class 参数、将来的 `Class.getName`/`forName`)。
- **桩**:`java.lang.Object` **不加载**——`linkSuperClass` 把 "java.lang.Object" 的 super 映射为 `nullptr`;`Object.\<init\>()V` 被拦截为 no-op。

---

## native 与拦截机制

两种"内建行为",**刻意分开、生命周期相反,不合并**:

- **`NativeRegistry`**(engine)——**永久、正规**:给**已加载类**里的 `ACC_NATIVE` 方法用。INVOKESTATIC 等 resolve **之后**查 `isNative` → 用 key(来自已解析 `Method`,`Klass::getName()` 斜杠形式)查表 → 调 `NativeFn(OperandStack&)`。这是 JNI 式 native 绑定的简化版,**会增长并正规化**;未来向 JNI 靠拢(fn 签名从裸 `OperandStack&` 演进成 `JNIEnv*` + 编组参数)。
- **符号拦截**(`StubIntercepts`)——**临时 workaround、应烧毁归零**:给**未加载**的 JDK 类(System.out、PrintStream.println、Object.\<init\>)用,它们无法 resolve。resolve **之前** peek SymRef,匹配 `{class_name(点), member, descriptor}` → handler。

**为什么不合并**:两者命运相反——native 越做越多,拦截应随真类库/桩类到位而**逐条删除**。每个拦截最终分解成**(真方法分派)+(永久 native)**:如 `println` 拦截 → 有了已加载 PrintStream 后变成真方法调用,其底层 `write` 落在 NativeRegistry;`Object.\<init\>` 拦截 → 加载真·最小 Object 后变普通调用;`System.out` 拦截 → 真 System 类 + `\<clinit\>` 装配 PrintStream 后走正常 GETSTATIC。绑一张表会让拦截无法独立烧毁。

**key 形式**:R2 后两套 key 都是**斜杠**(拦截 key 来自 `RuntimeConstantPool::symbolicKey`,native key 来自 `Klass::getName()`,都是斜杠)。**已实现**:`StubIntercepts` 表(engine)+ `symbolicKey`(oops,resolve 后返回 nullopt、且对已解析的目标类返回 nullopt 不抛)+ `tryStubIntercept` 三站点统一,顶部维护 burn-down 列表。

---

## String(桩)

`LDC` 把字面量驻留进 `StringPool`(oops,结点式 `unordered_set` → 地址稳定),压一个稳定 `std::string*` 作为 `Jref`。**不是真 String Object**(无 Klass)——对桩接 println 路径够用。真 String(`char[]` 后端)推迟;字符串**运算**可作为 native/拦截 handler 直接操作 std::string(JDK 8 目标 → `+` 走 StringBuilder,而非 invokedynamic)。

---

## 边界检查与索引类型

**在信任边界做检查**(不可信输入),因为还没有字节码校验器(校验器会把这些提前到一次性阶段):
- class 文件解析(`ByteReader`):已查。
- 操作数驱动的容器访问(CP 索引、`BytecodeReader` 读操作数、局部变量访问):已查(`.at()` / 显式 throw)。这些**替代缺失的校验器**;有了校验器后可降级为 assert。
- VM 内部索引(字段槽索引):本该 assert(良构程序不会越界);当前用 `.at()`。

**索引类型**:`U2` 给 **class 文件声明、且上界为 u2** 的量(CP 索引——`constant_pool_count` 是 u2;局部变量号——`max_locals` 是 u2);`size_t` 给**运行期容器索引、上界非 u2** 的(实例/静态字段槽可超 u2:65535 字段 × 2 槽)。**规矩:索引类型 = 它所索引容器的大小上界**。

---

## 错误处理

失败抛 `std::runtime_error`(未绑定 native、NPE、非法 opcode、解析失败、除零)。这会**打挂 VM**,而非建模 Java 异常——推迟到异常处理(ATHROW + 异常表 + Java 异常对象)落地。

---

## `<clinit>` 类初始化

**决策**:`Klass` 带初始化状态机 `Allocated→Loaded→Linked→BeingInitialized→FullyInitialized(→InitializationError)`。NEW/GETSTATIC/PUTSTATIC/INVOKESTATIC 在目标类 `state==Linked` 时:**回退触发指令 PC** → `initialize()` 压 `<clinit>()V` 帧(super 先于 sub)→ 触发指令随后重执行(此时已 `FullyInitialized`)。`<clinit>` 的 RETURN 处 `markFullyInitialized`。带 ConstantValue 的 `static final` 常量在 prepare 阶段直接写入(早于 `<clinit>`)。

**已修复缺陷(有回归测试 `interpreter_init_type_regression_test` 覆盖)**:
- **B1**(已修) — 无 `<clinit>` 未压帧时,触发点 `pc` 应回退为当前帧的 PC。
- **B2**(已修) — `isInstanceOf` 沿继承/接口链递归判断。
- **B3**(已修) — `<clinit>` 用 `findMethod("<clinit>","()V", false)` 限定本类,不误继承父类。
- **B4**(已修) — 父子 `<clinit>` 执行顺序:super 先于 sub。
- **B5**(已修) — 无自身 `<clinit>` 的子类,`initialize()` 直接置 `FullyInitialized` 后**无条件**递归 `super.initialize()`,保证父类 `<clinit>` 被触发。

---

## 已知限制 / 推迟项

- 无 GC(bump arena 全泄漏——短程序无所谓)。
- **数组**:Phase A(klass/路由 + 数组 CHECKCAST/INSTANCEOF)完成;Phase B–E(堆分配/创建/访问)待做,详见数组一节。
- 无 Java 层异常(失败抛 `std::runtime_error` 打挂 VM);无 INVOKEDYNAMIC。
- String 是 std::string 桩;无真类库;`java.lang.Object` 被桩(super 置空)。
- 单线程;无 monitor。
- 字符串驻留仅覆盖常量池字面量(`StringPool` 去重 + LDC 稳定指针);`String.intern()` 与运行时字符串(拼接 `new String`)未实现。
