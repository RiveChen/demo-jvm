package tests.data.java;

/**
 * @file Showcase.java
 * @brief Comprehensive demo exercising all JVM features supported by demo-jvm.
 *
 * Covers: constants, loads/stores, arithmetic, bitwise, type conversions,
 * comparisons/branches, switches, object allocation, field access,
 * String methods, static utilities (Math / Integer / System),
 * and print/println family (int, long, float, double, boolean, char, String).
 *
 * Each demo is a separate static method returning a value for individual testing.
 * The main() method runs all demos and prints results via the stubbed
 * System.out.println (all primitive types now supported).
 */
public class Showcase {

    // ============================================================
    // 1. Constants + Arithmetic (IADD, ISUB, IMUL, IDIV, IREM, INEG)
    // ============================================================
    static int arithmeticDemo() {
        int a = 100;
        int b = 7;
        int sum  = a + b;          // IADD
        int diff = a - b;          // ISUB
        int prod = a * b;          // IMUL
        int quot = a / b;          // IDIV
        int rem  = a % b;          // IREM
        int neg  = -rem;           // INEG
        return sum + diff + prod + quot + rem + neg;   // expected: 107 + 93 + 700 + 14 + 2 + (-2) = 914
    }

    // ============================================================
    // 2. Load/Store + IINC
    // ============================================================
    static int loadStoreDemo() {
        int x = 10;     // ISTORE
        int y = 20;     // ISTORE (different slot)
        x = x + 5;      // ILOAD, IADD, ISTORE
        y++;            // IINC
        int z = x + y;  // ILOAD, ILOAD, IADD, ISTORE
        return z;       // expected: (10+5) + (20+1) = 36
    }

    // ============================================================
    // 3. Long Arithmetic (LCONST, LLOAD, LADD, LMUL, LDIV)
    // ============================================================
    static long longDemo() {
        long a = 1000L;
        long b = 7L;
        long sum  = a + b;         // LADD
        long prod = a * b;         // LMUL
        long quot = a / b;         // LDIV
        long rem  = a % b;         // LREM
        return sum + prod + quot + rem;  // expected: 1007 + 7000 + 142 + 6 = 8155
    }

    // ============================================================
    // 4. Float + Double (FCONST, FLOAD, FADD, DCONST, DLOAD, DCMPL)
    // ============================================================
    static double floatDoubleDemo() {
        float  fa = 3.5f;
        float  fb = 2.0f;
        double da = 10.0;
        double db = 3.0;

        float  fsum  = fa + fb;        // FADD
        double dsum  = da + db;        // DADD
        double dquot = da / db;        // DDIV
        double dprod = da * db;        // DMUL

        double result = (double)fsum + dsum + dquot + dprod;
        // fsum=5.5, dsum=13.0, dquot≈3.333..., dprod=30.0 → total ≈ 51.83333...
        return result;
    }

    // ============================================================
    // 5. Type Conversions (I2L, L2I, F2I, D2I, I2B, I2C, I2S)
    // ============================================================
    static int conversionDemo() {
        int     iv  = 257;
        long    lv  = iv;           // I2L
        int     i2  = (int)lv;      // L2I
        float   fv  = 99.7f;
        int     fi  = (int)fv;      // F2I (truncation: 99)
        double  dv  = 42.9;
        int     di  = (int)dv;      // D2I (truncation: 42)
        byte    bv  = (byte)iv;     // I2B (257 & 0xFF = 1, sign-extend → 1)
        char    cv  = (char)iv;     // I2C (257 → 257, zero-extend)
        short   sv  = (short)iv;    // I2S (257 → 257)
        return iv + i2 + fi + di + bv + cv + sv;
        // 257 + 257 + 99 + 42 + 1 + 257 + 257 = 1170
    }

    // ============================================================
    // 6. Bitwise Operations (ISHL, ISHR, IUSHR, IAND, IOR, IXOR)
    // ============================================================
    static int bitwiseDemo() {
        int a = 0x1234;         // 4660
        int b = 2;

        int shl  = a << b;      // ISHL:  0x48D0 = 18640
        int shr  = a >> b;      // ISHR:  0x048D = 1165
        int ushr = a >>> b;     // IUSHR: 0x048D = 1165  (positive same)
        int and  = a & 0xFF;    // IAND:  0x0034 = 52
        int or   = a | 0x000F;  // IOR:   0x123F = 4671
        int xor  = a ^ 0xFFFF;  // IXOR:  0xEDCB = -4405 (signed)
        return shl + shr + ushr + and + or + xor;
    }

    // ============================================================
    // 7. Comparisons / Branches (IFEQ, IFNE, IFLT, IFGE, IFGT, IFLE, GOTO)
    // ============================================================
    static int branchDemo() {
        int x = 5;
        int result = 0;

        if (x == 5)  result += 10;   // IFEQ (with zero, but jc uses x == 5 → IF_ICMPEQ)
        if (x != 0)  result += 20;   // IFNE
        if (x > 0)   result += 40;   // IFGT
        if (x < 10)  result += 80;   // IFLT actually x < 10 → IF_ICMPLT
        if (x >= 0)  result += 160;  // IFGE
        if (x <= 5)  result += 320;  // IFLE
        return result;               // expected: 10+20+40+80+160+320 = 630
    }

    // ============================================================
    // 8. TABLESWITCH (contiguous cases)
    // ============================================================
    static int tableSwitchDemo(int x) {
        switch (x) {
            case 1:  return 10;
            case 2:  return 20;
            case 3:  return 30;
            case 4:  return 40;
            case 5:  return 50;
            default: return 0;
        }
    }

    // ============================================================
    // 9. LOOKUPSWITCH (sparse cases)
    // ============================================================
    static int lookupSwitchDemo(int x) {
        switch (x) {
            case 10:   return 100;
            case 100:  return 1000;
            case 1000: return 10000;
            default:   return -1;
        }
    }

    // ============================================================
    // 10. Inner class for Object + Field access test
    // ============================================================
    static class Point {
        int x;
        int y;
    }

    static int objectDemo() {
        Point p = new Point();     // NEW, DUP, INVOKESPECIAL Object.<init>
        p.x = 30;                  // PUTFIELD (int)
        p.y = 12;                  // PUTFIELD (int)
        int sum = p.x + p.y;       // GETFIELD, GETFIELD, IADD
        return sum;                // expected: 42
    }

    // ============================================================
    // 11. Control flow: if-else chain
    // ============================================================
    static int ifElseDemo(int x) {
        if (x > 100) {
            return 1;
        } else if (x > 50) {
            return 2;
        } else if (x > 10) {
            return 3;
        } else {
            return 4;
        }
    }

    // ============================================================
    // 12. String methods (Tier 1 stub intercepts)
    // ============================================================
    static int stringDemo() {
        String s = "hello";
        int len = s.length();                     // String.length()
        char ch = s.charAt(1);                    // String.charAt()
        boolean empty = "".isEmpty();             // String.isEmpty()
        boolean eq = "abc".equals("abc");          // String.equals (true)
        boolean neq = "abc".equals("xyz");         // String.equals (false)
        int h = "abc".hashCode();                  // String.hashCode()
        int cmp = "abc".compareTo("abd");          // String.compareTo (< 0)
        return len + (int)ch + (empty ? 1 : 0) + (eq ? 1 : 0) + (neq ? 0 : 1) + h + cmp;
    }

    // ============================================================
    // 13. Static utility methods (Tier 1 stub intercepts)
    // ============================================================
    static long utilityDemo() {
        long now = System.currentTimeMillis();    // System.currentTimeMillis()
        int parsed = Integer.parseInt("42");      // Integer.parseInt()
        String str = Integer.toString(123);       // Integer.toString()
        int abs = Math.abs(-7);                   // Math.abs(int)
        long absL = Math.abs(-99L);               // Math.abs(long)
        float absF = Math.abs(-3.14f);             // Math.abs(float)
        double absD = Math.abs(-2.718);            // Math.abs(double)
        int mx = Math.max(10, 20);                // Math.max(int, int)
        int mn = Math.min(10, 20);                // Math.min(int, int)
        long mxL = Math.max(100L, 200L);           // Math.max(long, long)
        long mnL = Math.min(100L, 200L);           // Math.min(long, long)
        double sqrt = Math.sqrt(9.0);              // Math.sqrt(double)
        return now + parsed + abs + absL + mx + mn + mxL + mnL + (long)absD;
    }

    // ============================================================
    // 14. INVOKEINTERFACE demo
    // ============================================================
    interface Talker {
        int talk();
    }

    static class Cat implements Talker {
        public int talk() {
            return 7;
        }
    }

    static class DogSpeaker implements Talker {
        public int talk() {
            return 3;
        }
    }

    /** Invoke interface method (invokeinterface). */
    static int interfaceDemo() {
        Talker t = new Cat();
        return t.talk();  // invokeinterface — expected: 7
    }

    // ============================================================
    // 15. INSTANCEOF demo (uses Animal/Dog hierarchy)
    // ============================================================
    static int instanceOfDemo() {
        Animal a = new Dog();                   // Dog is-a Animal
        boolean r1 = a instanceof Animal;        // true → 1
        boolean r2 = a instanceof Dog;           // true → 1
        Point  p = null;
        boolean r3 = p instanceof Point;         // false → 0 (null check)
        Animal n = null;
        boolean r4 = n instanceof Animal;        // false → 0
        return (r1 ? 1 : 0) + (r2 ? 1 : 0) + (r3 ? 0 : 1) + (r4 ? 0 : 1);
        // 1 + 1 + 1 + 1 = 4
    }

    // ============================================================
    // 16. CHECKCAST demo (uses Animal/Dog hierarchy)
    // ============================================================
    static int checkCastDemo() {
        Animal a = new Dog();                   // Dog stored as Animal
        Dog d = (Dog) a;                        // checkcast Dog — valid
        return d.sound();                       // expected: 2
    }

    // ============================================================
    // Main — orchestrate all demos, print results
    // ============================================================
    public static void main(String[] args) {
        int total = 0;

        System.out.print("arithmeticDemo: ");
        System.out.println(arithmeticDemo());

        System.out.print("loadStoreDemo:   ");
        System.out.println(loadStoreDemo());

        System.out.print("longDemo:        ");
        System.out.println(longDemo());

        System.out.print("floatDoubleDemo: ");
        System.out.println(floatDoubleDemo());

        System.out.print("conversionDemo:  ");
        System.out.println(conversionDemo());

        System.out.print("bitwiseDemo:     ");
        System.out.println(bitwiseDemo());

        System.out.print("branchDemo:      ");
        System.out.println(branchDemo());

        System.out.print("tableSwitchDemo(3):  ");
        System.out.println(tableSwitchDemo(3));

        System.out.print("lookupSwitchDemo(100): ");
        System.out.println(lookupSwitchDemo(100));

        System.out.print("objectDemo:      ");
        System.out.println(objectDemo());

        System.out.print("ifElseDemo(25):  ");
        System.out.println(ifElseDemo(25));

        System.out.print("stringDemo:      ");
        System.out.println(stringDemo());

        System.out.print("utilityDemo:     ");
        System.out.println(utilityDemo());

        // Demonstrate boolean and char print
        System.out.print("flag=");
        System.out.println(true);

        System.out.print("letter=");
        System.out.println('Z');

        // Demonstrate System.err
        System.err.println("(stderr) Showcase done.");

        System.out.println("Showcase finished.");
    }
}