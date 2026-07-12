package tests.data.java;

public class MethodInvocationTest {
    // ============================================================================
    // Static method - Factorial implementation
    // ============================================================================
    public static int factorial(int n) {
        if (n <= 0) {
            return 1;
        }
        int result = 1;
        int i = 1;
        while (i <= n) {
            result = result * i;
            i++;
        }
        return result;
    }

    // ============================================================================
    // Test method that invokes static method using invokestatic
    // ============================================================================
    public static int testInvokeStaticFactorial(int n) {
        return factorial(n);
    }

    // ============================================================================
    // Category-2 (long/double) argument passing via invokestatic — exercises A2
    // Each testInvokeX is a wrapper whose bytecode does invokestatic X, so the
    // interpreter's INVOKESTATIC arg-transfer (setWide/popWide + offsets) is hit.
    // ============================================================================

    // long args, non-commutative → catches swapped ordering / corrupted value
    public static long subLong(long a, long b) {
        return a - b;
    }
    public static long testInvokeSubLong(long a, long b) {
        return subLong(a, b);
    }

    // double args
    public static double subDouble(double a, double b) {
        return a - b;
    }
    public static double testInvokeSubDouble(double a, double b) {
        return subDouble(a, b);
    }

    // returns the SECOND long → checks the second category-2 arg reaches slot 2
    public static long pickSecondLong(long a, long b) {
        return b;
    }
    public static long testInvokePickSecondLong(long a, long b) {
        return pickSecondLong(a, b);
    }

    // int, long, int → category-2 sandwiched between category-1, stresses offsets
    public static long packMix(int a, long b, int c) {
        return a * 1000000L + b * 1000L + c;
    }
    public static long testInvokePackMix(int a, long b, int c) {
        return packMix(a, b, c);
    }
}
