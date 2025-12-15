package tests.data.java;

public class KlassTestData {
    // instance fields
    public int x;
    public long y;
    public double dz;

    // static fields
    public static int sx;
    public static double sd;

    public KlassTestData() {
        sx = 99;
        sd = 3.14f;
    }

    public static void main(String[] args) {
        KlassTestData data = new KlassTestData();
        data.add(1, 2);
        staticAdd(42, 58);
    }

    public int add(int a, int b) {
        return a + b;
    }

    public static int staticAdd(int a, int b) {
        return a + b;
    }
}
