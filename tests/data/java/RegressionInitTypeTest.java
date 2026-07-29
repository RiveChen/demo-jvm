package tests.data.java;

/**
 * Regression fixtures for four latent bugs in class initialization / type checks:
 *
 *   B1  interpreter resets pc to 0 after initialize() when NO <clinit> frame is
 *       pushed  ->  work done before a no-<clinit> static trigger is re-executed.
 *   B2  InstanceKlass::isInstanceOf misses interfaces implemented by a SUPERCLASS.
 *   B3  <clinit> is looked up via findMethod (walks super chain), so a subclass
 *       without its own <clinit> inherits + re-runs the super's <clinit>.
 *   B4  super/sub <clinit> run in the wrong order (sub before super).
 *
 * All helper types are package-private on purpose (the loader does not check
 * access); only the driver class needs to be public to match the file name.
 */

// ---- B2: instanceof through a superclass-implemented interface -------------
interface RIT_Iface {}

class RIT_Impl implements RIT_Iface {}

class RIT_Sub extends RIT_Impl {}

// ---- B1: a no-<clinit> class used purely as a static-init trigger ----------
// No initializer => no <clinit>; V defaults to 0. Reading V is a GETSTATIC
// (non-final, so it is NOT constant-folded) that triggers initialization.
class RIT_NoClinit {
    static int V;
}

// Has an explicit <clinit>, so touching it uses the (correct) frame path and
// does not itself exercise B1 -- it is only the accumulator of the side effect.
class RIT_Acc {
    static int n;
    static { n = 0; }
}

// ---- B3 / B4: a two-level hierarchy whose SUPER has a <clinit> --------------
class RIT_Trace {
    static int seq;
    static { seq = 0; }
}

class RIT_Base {
    // contributes decimal digit 1
    static { RIT_Trace.seq = RIT_Trace.seq * 10 + 1; }
}

class RIT_Derived extends RIT_Base {
    // contributes decimal digit 2
    static { RIT_Trace.seq = RIT_Trace.seq * 10 + 2; }
}

// Subclass of a class that HAS a <clinit>, but with no <clinit> of its own.
class RIT_DerivedNoClinit extends RIT_Base {}

public class RegressionInitTypeTest {

    /**
     * B1. Increment a static on an already-initialized class, then read a static
     * of a class that has NO <clinit> (the init trigger). If the interpreter
     * resets pc to 0 instead of retrying just the trigger, the whole method
     * re-runs and the increment happens twice.
     *
     * Correct = 1, buggy = 2.
     */
    public static int b1NoRestart() {
        RIT_Acc.n = RIT_Acc.n + 1;   // side effect before the trigger
        int t = RIT_NoClinit.V;      // GETSTATIC of a no-<clinit> class => trigger
        return RIT_Acc.n + t;        // t == 0; using t keeps the GETSTATIC alive
    }

    /**
     * B2. RIT_Sub's superclass RIT_Impl implements RIT_Iface, so a RIT_Sub is an
     * RIT_Iface. (The NEW may itself hit B1, but that only re-allocates and does
     * not change the instanceof result, so this case isolates B2.)
     *
     * Correct = true, buggy = false.
     */
    public static boolean b2SubIsIface() {
        RIT_Sub s = new RIT_Sub();
        return s instanceof RIT_Iface;
    }

    /**
     * B3. Trigger initialization of a subclass that has no <clinit> of its own.
     * The C++ side then asserts the subclass reaches FullyInitialized. Under the
     * bug the super's <clinit> is found via findMethod and re-run, leaving the
     * subclass stuck in BeingInitialized (and, in debug builds, tripping the
     * markFullyInitialized assert).
     */
    public static int b3TriggerNoClinit() {
        new RIT_DerivedNoClinit();
        return 0;
    }

    /**
     * B4. NEW RIT_Derived triggers the <clinit> chain. RIT_Base must run before
     * RIT_Derived: Base contributes digit 1, Derived digit 2.
     *
     * Correct = 12, buggy (sub before super) = 21.
     */
    public static int b4SuperBeforeSub() {
        new RIT_Derived();
        return RIT_Trace.seq;
    }
}
