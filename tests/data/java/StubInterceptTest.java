package tests.data.java;

/**
 * @file StubInterceptTest.java
 * @brief Test data for Tier 1 stub intercepts.
 *
 * Each static method exercises a different stub intercept handler
 * (print/println, String methods, static utilities).
 */
public class StubInterceptTest {

    // ============================================================
    // PrintStream.print/println family
    // ============================================================

    /** Basic println(String) — already works, just verifying the framework. */
    static void printlnString() {
        System.out.println("hello");
    }

    /** println(int) */
    static void printlnInt() {
        System.out.println(42);
    }

    /** println(long) */
    static void printlnLong() {
        System.out.println(1234567890123L);
    }

    /** println(float) */
    static void printlnFloat() {
        System.out.println(3.14f);
    }

    /** println(double) */
    static void printlnDouble() {
        System.out.println(2.71828);
    }

    /** println(boolean) */
    static void printlnBoolean() {
        System.out.println(true);
        System.out.println(false);
    }

    /** println(char) */
    static void printlnChar() {
        System.out.println('A');
    }

    /** print(String) + print(int) — no trailing newline */
    static void printConcatenated() {
        System.out.print("value=");
        System.out.print(99);
    }

    /** System.err.println(String) */
    static void printlnErr() {
        System.err.println("err msg");
    }

    // ============================================================
    // String methods
    // ============================================================

    static int stringLength(String s) {
        return s.length();
    }

    static char stringCharAt(String s) {
        return s.charAt(1);  // second character
    }

    static boolean stringIsEmpty(String s) {
        return s.isEmpty();
    }

    static boolean stringEquals(String a, String b) {
        return a.equals(b);
    }

    static int stringHashCode(String s) {
        return s.hashCode();
    }

    static int stringCompareTo(String a, String b) {
        return a.compareTo(b);
    }

    // ============================================================
    // Static utility methods
    // ============================================================

    static long systemCurrentTimeMillis() {
        return System.currentTimeMillis();
    }

    static long systemNanoTime() {
        return System.nanoTime();
    }

    static int parseInt(String s) {
        return Integer.parseInt(s);
    }

    static String intToString(int n) {
        return Integer.toString(n);
    }

    static int mathAbsInt() {
        return Math.abs(-7);
    }

    static long mathAbsLong() {
        return Math.abs(-1234567890123L);
    }

    static int mathMax() {
        return Math.max(10, 20);
    }

    static int mathMin() {
        return Math.min(10, 20);
    }

    static double mathSqrt() {
        return Math.sqrt(9.0);
    }

    // ============================================================
    // Minimal main — just calls a subset and prints a summary
    // ============================================================
    public static void main(String[] args) {
        System.out.println("=== Demo ===");
        System.out.println(42);
        System.out.println(3.14);
        System.out.println(true);
        System.out.println('Z');
        System.out.println("length=" + "hello".length());
        System.out.println(Integer.parseInt("123") + 1);
        System.out.println("Done.");
    }
}