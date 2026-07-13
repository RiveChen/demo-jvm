package tests.data.java;

/**
 * @file TypeCheckTest.java
 * @brief Test data for CHECKCAST and INSTANCEOF instructions.
 *
 * Uses the existing Animal/Dog hierarchy:
 *   Animal ← Dog
 * Also tests: null reference, self-reference, interface cast.
 */
public class TypeCheckTest {

    /** instanceof: Dog is an Animal. */
    public static boolean instanceOfDogIsAnimal() {
        Animal a = new Dog();
        return a instanceof Animal;
    }

    /** instanceof: Dog is a Dog. */
    public static boolean instanceOfDogIsDog() {
        Animal a = new Dog();
        return a instanceof Dog;
    }

    /** instanceof: Animal is NOT a Dog (parent not a child). */
    public static boolean instanceOfAnimalIsNotDog() {
        Animal a = new Animal();
        return a instanceof Dog;
    }

    /** instanceof: null is nothing. */
    public static boolean instanceOfNull() {
        Animal a = null;
        return a instanceof Animal;
    }

    /** checkcast: cast Dog to Animal should succeed. */
    public static Dog checkcastAnimalToDog() {
        Animal a = new Dog();
        return (Dog)a;  // checkcast Dog
    }

    /** checkcast: cast Animal to Dog should fail (ClassCastException). */
    public static Dog checkcastFail() {
        Animal a = new Animal();
        return (Dog)a;  // checkcast Dog → throws
    }

    /** checkcast: null is always safe. */
    public static Dog checkcastNull() {
        Animal a = null;
        return (Dog)a;  // checkcast null → null
    }
}