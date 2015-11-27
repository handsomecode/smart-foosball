package is.handsome.foosballserver;

public class ReceivedNewScoreEvent {

    private String score_1;
    private String score_2;

    public ReceivedNewScoreEvent(String score_1, String score_2) {
        this.score_1 = score_1;
        this.score_2 = score_2;
    }

    public String getScore_1() {
        return score_1;
    }


    public String getScore_2() {
        return score_2;
    }
}
