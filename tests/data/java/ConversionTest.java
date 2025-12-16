package tests.data.java;

public class ConversionTest {
    // Int to Long
    public static long testI2L(int a) {
        return (long) a;
    }

    // Int to Float
    public static float testI2F(int a) {
        return (float) a;
    }

    // Int to Double
    public static double testI2D(int a) {
        return (double) a;
    }

    // Long to Int
    public static int testL2I(long a) {
        return (int) a;
    }

    // Long to Float
    public static float testL2F(long a) {
        return (float) a;
    }

    // Long to Double
    public static double testL2D(long a) {
        return (double) a;
    }

    // Float to Int
    public static int testF2I(float a) {
        return (int) a;
    }

    // Float to Long
    public static long testF2L(float a) {
        return (long) a;
    }

    // Float to Double
    public static double testF2D(float a) {
        return (double) a;
    }

    // Double to Int
    public static int testD2I(double a) {
        return (int) a;
    }

    // Double to Long
    public static long testD2L(double a) {
        return (long) a;
    }

    // Double to Float
    public static float testD2F(double a) {
        return (float) a;
    }

    // Int to Byte
    public static int testI2B(int a) {
        return (byte) a;
    }

    // Int to Char
    public static int testI2C(int a) {
        return (char) a;
    }

    // Int to Short
    public static int testI2S(int a) {
        return (short) a;
    }

    // Float NaN to Int
    public static int testF2I_NaN() {
        float nan = Float.NaN;
        return (int) nan;
    }

    // Float Infinity to Int
    public static int testF2I_Infinity() {
        float inf = Float.POSITIVE_INFINITY;
        return (int) inf;
    }

    // Double NaN to Int
    public static int testD2I_NaN() {
        double nan = Double.NaN;
        return (int) nan;
    }

    // Double Infinity to Int
    public static int testD2I_Infinity() {
        double inf = Double.POSITIVE_INFINITY;
        return (int) inf;
    }
}
