package is.handsome.foosballserver;

public class Score {
    private int sideA;
    private int sideB;

    public void reset() {
        this.sideA = 0;
        this.sideB = 0;
    }

    public int getSideA() {
        return sideA;
    }

    public void increaseSideA() {
        sideA++;
    }

    public void decreaseSideA() {
        if (sideA > 0) {
            sideA--;
        }
    }

    public void increaseSideB() {
        sideB++;
    }

    public void decreaseSideB() {
        if (sideB > 0) {
            sideB--;
        }
    }

    public int getSideB() {
        return sideB;
    }
}
