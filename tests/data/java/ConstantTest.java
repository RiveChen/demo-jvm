package tests.data.java;

public class ConstantTest {
    // Integer constants
    public static int testICONST_M1() {
        return -1;
    }

    public static int testICONST_0() {
        return 0;
    }

    public static int testICONST_1() {
        return 1;
    }

    public static int testICONST_2() {
        return 2;
    }

    public static int testICONST_3() {
        return 3;
    }

    public static int testICONST_4() {
        return 4;
    }

    public static int testICONST_5() {
        return 5;
    }

    // Long constants
    public static long testLCONST_0() {
        return 0L;
    }

    public static long testLCONST_1() {
        return 1L;
    }

    // Float constants
    public static float testFCONST_0() {
        return 0.0f;
    }

    public static float testFCONST_1() {
        return 1.0f;
    }

    public static float testFCONST_2() {
        return 2.0f;
    }

    // Double constants
    public static double testDCONST_0() {
        return 0.0;
    }

    public static double testDCONST_1() {
        return 1.0;
    }

    // BIPUSH tests
    public static int testBIPUSH_127() {
        return 127;
    }

    public static int testBIPUSH_Neg128() {
        return -128;
    }

    public static int testBIPUSH_0() {
        return 0;
    }

    // SIPUSH tests
    public static int testSIPUSH_32767() {
        return 32767;
    }

    public static int testSIPUSH_Neg32768() {
        return -32768;
    }

    public static int testSIPUSH_100() {
        return 100;
    }

    // ACONST_NULL
    public static Object testACONST_NULL() {
        return null;
    }
}
