package is.handsome.foosballserver.server;

import android.content.Context;
import android.content.Intent;
import android.support.v4.content.LocalBroadcastManager;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import fi.iki.elonen.NanoHTTPD;

public class Server extends NanoHTTPD {
    private final static int PORT = 8080;

    private Context context;

    public static final String SCORE_A = "scoreA";
    public static final String SCORE_B = "scoreB";
    public static final String INTENT_FILTER_ACTION = "score_received";
    public static final String RESET = "reset_score";
    public static final int RESET_COMMAND = 1;
    public static final String SIDE = "side";
    public static final int SIDE_A = 0;
    public static final int SIDE_B = 1;
    public static final int SIDE_UNDEFINED = 3;

    public Server(Context context) throws IOException {
        super(PORT);
        this.context = context;
        start();
        System.out.println("\nRunning! Point your browers to http://localhost:8080/ \n");
    }

    @Override
    public Response serve(IHTTPSession session) {
        Map<String, String> files = new HashMap<>();
        try {
            session.parseBody(files);
            String restartCommand = session.getParms().get("restart");
            String side = session.getParms().get("side");
            if (restartCommand != null && restartCommand.equals("true")) {
                System.out.println("reset score");
                sendResetScoreCommand();
            } if(side != null) {
                int s = side.equals("A") ? SIDE_A : side.equals("B") ? SIDE_B : SIDE_UNDEFINED;
                sendReceivedData(s);
            } else {
                String score_1 = session.getParms().get("score_1");
                String score_2 = session.getParms().get("score_2");
                System.out.println("score_1 =" + score_1);
                System.out.println("score_2 =" + score_2);
                sendReceivedData(score_1, score_2);
            }
        } catch (IOException e) {
            return newFixedLengthResponse(Response.Status.INTERNAL_ERROR, MIME_PLAINTEXT, "SERVER INTERNAL ERROR: IOException: " + e.getMessage());
        } catch (ResponseException e) {
            System.out.println("responseError");
            return newFixedLengthResponse(e.getStatus(), MIME_PLAINTEXT, e.getMessage());
        }

        String msg = "<html><body><h1>HTTP /1.1 200 OK</h1>\n";
        msg += "<p>New score has been received successfully " + session.getUri() + " !</p>";
        return newFixedLengthResponse(msg + "</body></html>\n");
    }

    private void sendResetScoreCommand() {
        Intent intent = new Intent(INTENT_FILTER_ACTION);
        intent.putExtra(RESET, RESET_COMMAND);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    private void sendReceivedData(String scoreA, String scoreB) {
        Intent intent = new Intent(INTENT_FILTER_ACTION);
        intent.putExtra(SCORE_A, scoreA);
        intent.putExtra(SCORE_B, scoreB);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    private void sendReceivedData(int side) {
        Intent intent = new Intent(INTENT_FILTER_ACTION);
        intent.putExtra(SIDE, side);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }
}
