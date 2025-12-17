package tests.data.java;

public class ControlFlowTest {
    // ============================================================================
    // IFEQ - Branch if int value equals 0
    // ============================================================================
    public static int testIFEQ_True(int value) {
        if (value == 0) {
            return 1;
        }
        return 0;
    }

    public static int testIFEQ_False(int value) {
        if (value == 0) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IFNE - Branch if int value not equal to 0
    // ============================================================================
    public static int testIFNE_True(int value) {
        if (value != 0) {
            return 1;
        }
        return 0;
    }

    public static int testIFNE_False(int value) {
        if (value != 0) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IFLT - Branch if int value less than 0
    // ============================================================================
    public static int testIFLT_True(int value) {
        if (value < 0) {
            return 1;
        }
        return 0;
    }

    public static int testIFLT_False(int value) {
        if (value < 0) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IFGE - Branch if int value greater than or equal to 0
    // ============================================================================
    public static int testIFGE_True(int value) {
        if (value >= 0) {
            return 1;
        }
        return 0;
    }

    public static int testIFGE_False(int value) {
        if (value >= 0) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IFGT - Branch if int value greater than 0
    // ============================================================================
    public static int testIFGT_True(int value) {
        if (value > 0) {
            return 1;
        }
        return 0;
    }

    public static int testIFGT_False(int value) {
        if (value > 0) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IFLE - Branch if int value less than or equal to 0
    // ============================================================================
    public static int testIFLE_True(int value) {
        if (value <= 0) {
            return 1;
        }
        return 0;
    }

    public static int testIFLE_False(int value) {
        if (value <= 0) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IF_ICMPEQ - Branch if two int values are equal
    // ============================================================================
    public static int testIF_ICMPEQ_True(int a, int b) {
        if (a == b) {
            return 1;
        }
        return 0;
    }

    public static int testIF_ICMPEQ_False(int a, int b) {
        if (a == b) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IF_ICMPNE - Branch if two int values are not equal
    // ============================================================================
    public static int testIF_ICMPNE_True(int a, int b) {
        if (a != b) {
            return 1;
        }
        return 0;
    }

    public static int testIF_ICMPNE_False(int a, int b) {
        if (a != b) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IF_ICMPLT - Branch if first int value less than second
    // ============================================================================
    public static int testIF_ICMPLT_True(int a, int b) {
        if (a < b) {
            return 1;
        }
        return 0;
    }

    public static int testIF_ICMPLT_False(int a, int b) {
        if (a < b) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IF_ICMPGE - Branch if first int value greater than or equal to second
    // ============================================================================
    public static int testIF_ICMPGE_True(int a, int b) {
        if (a >= b) {
            return 1;
        }
        return 0;
    }

    public static int testIF_ICMPGE_False(int a, int b) {
        if (a >= b) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IF_ICMPGT - Branch if first int value greater than second
    // ============================================================================
    public static int testIF_ICMPGT_True(int a, int b) {
        if (a > b) {
            return 1;
        }
        return 0;
    }

    public static int testIF_ICMPGT_False(int a, int b) {
        if (a > b) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IF_ICMPLE - Branch if first int value less than or equal to second
    // ============================================================================
    public static int testIF_ICMPLE_True(int a, int b) {
        if (a <= b) {
            return 1;
        }
        return 0;
    }

    public static int testIF_ICMPLE_False(int a, int b) {
        if (a <= b) {
            return 0;
        }
        return 1;
    }

    // ============================================================================
    // IFNULL - Branch if reference value is null
    // TODO: Uncomment when object support is implemented
    // ============================================================================
    /*
     * public static int testIFNULL_True(Object obj) {
     * if (obj == null) {
     * return 1;
     * }
     * return 0;
     * }
     * 
     * public static int testIFNULL_False(Object obj) {
     * if (obj == null) {
     * return 0;
     * }
     * return 1;
     * }
     */

    // ============================================================================
    // IFNONNULL - Branch if reference value is not null
    // TODO: Uncomment when object support is implemented
    // ============================================================================
    /*
     * public static int testIFNONNULL_True(Object obj) {
     * if (obj != null) {
     * return 1;
     * }
     * return 0;
     * }
     * 
     * public static int testIFNONNULL_False(Object obj) {
     * if (obj != null) {
     * return 0;
     * }
     * return 1;
     * }
     */

    // ============================================================================
    // IF_ACMPEQ - Branch if two reference values are equal
    // TODO: Uncomment when object support is implemented
    // ============================================================================
    /*
     * public static int testIF_ACMPEQ_True(Object a, Object b) {
     * if (a == b) {
     * return 1;
     * }
     * return 0;
     * }
     * 
     * public static int testIF_ACMPEQ_False(Object a, Object b) {
     * if (a == b) {
     * return 0;
     * }
     * return 1;
     * }
     */

    // ============================================================================
    // IF_ACMPNE - Branch if two reference values are not equal
    // TODO: Uncomment when object support is implemented
    // ============================================================================
    /*
     * public static int testIF_ACMPNE_True(Object a, Object b) {
     * if (a != b) {
     * return 1;
     * }
     * return 0;
     * }
     * 
     * public static int testIF_ACMPNE_False(Object a, Object b) {
     * if (a != b) {
     * return 0;
     * }
     * return 1;
     * }
     */

    // ============================================================================
    // GOTO - Unconditional branch
    // ============================================================================
    public static int testGOTO(int value) {
        if (value > 0) {
            return 10;
        }
        return 20;
    }

    // ============================================================================
    // TABLESWITCH - Table switch statement
    // ============================================================================
    public static int testTABLESWITCH(int value) {
        switch (value) {
            case 0:
                return 100;
            case 1:
                return 200;
            case 2:
                return 300;
            case 3:
                return 400;
            default:
                return 0;
        }
    }

    public static int testTABLESWITCH_Default(int value) {
        switch (value) {
            case 0:
                return 100;
            case 1:
                return 200;
            case 2:
                return 300;
            default:
                return 999;
        }
    }

    // ============================================================================
    // LOOKUPSWITCH - Lookup switch statement
    // ============================================================================
    public static int testLOOKUPSWITCH(int value) {
        switch (value) {
            case 10:
                return 1000;
            case 20:
                return 2000;
            case 30:
                return 3000;
            default:
                return 0;
        }
    }

    public static int testLOOKUPSWITCH_Default(int value) {
        switch (value) {
            case 10:
                return 1000;
            case 20:
                return 2000;
            case 30:
                return 3000;
            default:
                return 9999;
        }
    }

    // ============================================================================
    // Complex control flow - nested if statements
    // ============================================================================
    public static int testNestedIf(int a, int b) {
        if (a > 0) {
            if (b > 0) {
                return 1;
            } else {
                return 2;
            }
        } else {
            if (b > 0) {
                return 3;
            } else {
                return 4;
            }
        }
    }

    // ============================================================================
    // Complex control flow - while loop with break
    // ============================================================================
    public static int testWhileLoop(int count) {
        int result = 0;
        while (count > 0) {
            result += count;
            count--;
        }
        return result;
    }

    // ============================================================================
    // Factorial - Test complex control flow with multiplication
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
}
