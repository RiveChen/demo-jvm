package tests.data.java;

/**
 * Test class designed to verify <clinit> behavior.
 *
 * static final fields with compile-time constants become ConstantValue attributes
 * (set during preparation, before <clinit>).
 *
 * static int fields with non-constant initializers become part of <clinit>.
 */
public class ClinitTest {

    // ConstantValue (set in preparation, before <clinit>)
    public static final int CONST_INT = 42;
    public static final long CONST_LONG = 123456789L;
    public static final double CONST_DOUBLE = 3.14159;

    // Initialized in <clinit>
    public static int staticIntFromClinit;
    public static long staticLongFromClinit;

    // <clinit> block — runs when class is first actively used
    static {
        staticIntFromClinit = 100;
        staticLongFromClinit = 200L;
    }

    // ============================================================
    // Test methods
    // ============================================================

    /** Read a ConstantValue static final (should work without <clinit>). */
    public static int getConstInt() {
        return CONST_INT;
    }

    /** Read a field initialized by <clinit>. Triggers <clinit> via GETSTATIC. */
    public static int getIntFromClinit() {
        return staticIntFromClinit;
    }

    /** Read a long field initialized by <clinit>. */
    public static long getLongFromClinit() {
        return staticLongFromClinit;
    }

    /** Write then read an int. Triggers <clinit> via PUTSTATIC+GETSTATIC. */
    public static int putThenGetInt(int value) {
        staticIntFromClinit = value;
        return staticIntFromClinit;
    }
}