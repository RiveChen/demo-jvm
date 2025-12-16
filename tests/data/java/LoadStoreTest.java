package tests.data.java;

public class LoadStoreTest {
    // ILOAD tests
    public static int testILOAD(int a) {
        return a;
    }

    public static int testILOAD_0(int a) {
        return a;
    }

    public static int testILOAD_1(int a, int b) {
        return b;
    }

    public static int testILOAD_2(int a, int b, int c) {
        return c;
    }

    public static int testILOAD_3(int a, int b, int c, int d) {
        return d;
    }

    // ISTORE tests
    public static int testISTORE(int a) {
        int b = a;
        return b;
    }

    public static int testISTORE_0(int a) {
        int b = a;
        return b;
    }

    public static int testISTORE_1(int a, int b) {
        int c = b;
        return c;
    }

    public static int testISTORE_2(int a, int b, int c) {
        int d = c;
        return d;
    }

    public static int testISTORE_3(int a, int b, int c, int d) {
        int e = d;
        return e;
    }

    // LLOAD tests
    public static long testLLOAD(long a) {
        return a;
    }

    public static long testLLOAD_0(long a) {
        return a;
    }

    public static long testLLOAD_1(long a, long b) {
        return b;
    }

    // LSTORE tests
    public static long testLSTORE(long a) {
        long b = a;
        return b;
    }

    public static long testLSTORE_0(long a) {
        long b = a;
        return b;
    }

    // FLOAD tests
    public static float testFLOAD(float a) {
        return a;
    }

    public static float testFLOAD_0(float a) {
        return a;
    }

    public static float testFLOAD_1(float a, float b) {
        return b;
    }

    // FSTORE tests
    public static float testFSTORE(float a) {
        float b = a;
        return b;
    }

    public static float testFSTORE_0(float a) {
        float b = a;
        return b;
    }

    // DLOAD tests
    public static double testDLOAD(double a) {
        return a;
    }

    public static double testDLOAD_0(double a) {
        return a;
    }

    public static double testDLOAD_1(double a, double b) {
        return b;
    }

    // DSTORE tests
    public static double testDSTORE(double a) {
        double b = a;
        return b;
    }

    public static double testDSTORE_0(double a) {
        double b = a;
        return b;
    }

    // ALOAD tests
    public static Object testALOAD(Object a) {
        return a;
    }

    public static Object testALOAD_0(Object a) {
        return a;
    }

    public static Object testALOAD_1(Object a, Object b) {
        return b;
    }

    // ASTORE tests
    public static Object testASTORE(Object a) {
        Object b = a;
        return b;
    }

    public static Object testASTORE_0(Object a) {
        Object b = a;
        return b;
    }
}
