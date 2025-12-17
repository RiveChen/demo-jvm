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
}
