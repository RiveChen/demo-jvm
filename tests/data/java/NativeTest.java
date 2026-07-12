package tests.data.java;

public class NativeTest {
    // native method (no body) -> loaded as ACC_NATIVE, dispatched via NativeRegistry
    public static native int echo(int x);

    // bytecode wrapper: INVOKESTATIC echo -> native dispatch
    public static int callEcho(int x) {
        return echo(x);
    }
}
