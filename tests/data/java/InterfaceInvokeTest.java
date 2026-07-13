package tests.data.java;

/**
 * @file InterfaceInvokeTest.java
 * @brief Test data for INVOKEINTERFACE instruction.
 *
 * Defines an interface and an implementing class so the compiler
 * generates invokeinterface bytecode.
 */
public class InterfaceInvokeTest {

    /** A simple interface with a method. */
    public interface Greeter {
        int greet();
    }

    /** First implementation. */
    public static class HelloGreeter implements Greeter {
        @Override
        public int greet() {
            return 42;
        }
    }

    /** Second implementation (different return value). */
    public static class HiGreeter implements Greeter {
        @Override
        public int greet() {
            return 99;
        }
    }

    /** Invoke interface method via the interface type. */
    public static int invokeInterfaceGreet() {
        Greeter g = new HelloGreeter();
        return g.greet();  // invokeinterface
    }

    /** Invoke interface method — second impl. */
    public static int invokeInterfaceGreetHi() {
        Greeter g = new HiGreeter();
        return g.greet();  // invokeinterface
    }

    /** Invoke interface method with arguments (concat strings). */
    public static String invokeInterfaceToString() {
        Greeter g = new HelloGreeter();
        return "value=" + g.greet();  // invokeinterface + StringBuilder
    }
}