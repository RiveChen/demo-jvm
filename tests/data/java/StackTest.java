package tests.data.java;

public class StackTest {
    // POP test - simple addition then return
    public static int testPOP(int a, int b) {
        return a + b;
    }

    // POP2 test (for long/double) - simple addition then return
    public static long testPOP2(long a, long b) {
        return a + b;
    }

    // DUP test - duplicate value on stack
    public static int testDUP(int a) {
        return a + a;
    }

    // DUP_X1 test - duplicate top value under second value
    public static int testDUP_X1(int a, int b) {
        return a + b + a;
    }

    // DUP_X2 test - duplicate top value under third value
    public static int testDUP_X2(int a, int b, int c) {
        return a + b + c + a;
    }

    // DUP2 test - duplicate long value
    public static long testDUP2(long a) {
        return a + a;
    }

    // SWAP test - swap two values
    public static int testSWAP(int a, int b) {
        return b + a;
    }
}
