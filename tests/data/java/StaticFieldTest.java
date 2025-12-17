package tests.data.java;

public class StaticFieldTest {
    // ============================================================================
    // Static fields for testing PUTSTATIC and GETSTATIC
    // ============================================================================
    public static int staticIntField;
    public static long staticLongField;
    public static float staticFloatField;
    public static double staticDoubleField;

    // ============================================================================
    // Test methods for PUTSTATIC and GETSTATIC
    // ============================================================================

    // Test GETSTATIC with int
    public static int testGetStaticInt() {
        return staticIntField;
    }

    // Test PUTSTATIC with int, then GETSTATIC
    public static int testPutStaticInt(int value) {
        staticIntField = value;
        return staticIntField;
    }

    // Test GETSTATIC with long
    public static long testGetStaticLong() {
        return staticLongField;
    }

    // Test PUTSTATIC with long, then GETSTATIC
    public static long testPutStaticLong(long value) {
        staticLongField = value;
        return staticLongField;
    }

    // Test GETSTATIC with float
    public static float testGetStaticFloat() {
        return staticFloatField;
    }

    // Test PUTSTATIC with float, then GETSTATIC
    public static float testPutStaticFloat(float value) {
        staticFloatField = value;
        return staticFloatField;
    }

    // Test GETSTATIC with double
    public static double testGetStaticDouble() {
        return staticDoubleField;
    }

    // Test PUTSTATIC with double, then GETSTATIC
    public static double testPutStaticDouble(double value) {
        staticDoubleField = value;
        return staticDoubleField;
    }

    // Test multiple PUTSTATIC and GETSTATIC operations
    public static int testMultipleOperations(int a, int b) {
        staticIntField = a;
        int temp = staticIntField;
        staticIntField = b;
        return staticIntField + temp;
    }

    // Test initial value (should be 0 for numeric types)
    public static int testInitialValue() {
        return staticIntField;
    }
}
