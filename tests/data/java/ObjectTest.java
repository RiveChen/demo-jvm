package tests.data.java;

// Exercises NEW + INVOKESPECIAL(<init>, with Object.<init> stubbed as no-op)
// + PUTFIELD + GETFIELD end to end. Each testX wrapper is a static method so
// the interpreter test harness can drive it.
public class ObjectTest {
    int    ix;
    long   lx;
    double dx;

    // int field: NEW -> <init> -> putfield -> getfield
    public static int testIntField(int v) {
        ObjectTest o = new ObjectTest();
        o.ix = v;
        return o.ix;
    }

    // long field (category-2): value must survive the two-slot field
    public static long testLongField(long v) {
        ObjectTest o = new ObjectTest();
        o.lx = v;
        return o.lx;
    }

    // double field (category-2)
    public static double testDoubleField(double v) {
        ObjectTest o = new ObjectTest();
        o.dx = v;
        return o.dx;
    }

    // two distinct instances must not alias
    public static int testTwoObjects(int a, int b) {
        ObjectTest o1 = new ObjectTest();
        ObjectTest o2 = new ObjectTest();
        o1.ix = a;
        o2.ix = b;
        return o1.ix * 1000 + o2.ix;
    }
}
