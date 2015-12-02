package is.handsome.foosballserver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.SoundPool;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.text.format.Formatter;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.codetroopers.betterpickers.radialtimepicker.RadialTimePickerDialogFragment;

import java.io.IOException;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;
import is.handsome.foosballserver.server.Server;

public class MainActivity extends AppCompatActivity {

    private static final int GAME_TIME_DEFAULT = 5 * 60 * 1000 + 12 * 1000;
    private static final int UPDATE_INTERVAL = 1000;
    private static final boolean NOT_RUN_AFTER_CREATION = false;

    @Bind(R.id.score_A_text_view) TextView scoreATextView;
    @Bind(R.id.score_B_text_view) TextView scoreBTextView;
    @Bind(R.id.ip_address_text_view) TextView ipAddressTextView;
    @Bind(R.id.game_timer_text_view) TextView gameTimerTextView;

    private SoundPool soundPool;
    boolean soundLoaded;
    private int soundId;

    private Server server;
    private Score score;

    private CountDownTimerWithPause countDownTimerWithPause;

    private BroadcastReceiver dataReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            int side = intent.getIntExtra(Server.SIDE, -1);
            if (side == Server.SIDE_A) {
                if (countDownTimerWithPause.timeLeft() != 0) {
                    score.increaseSideA();
                    updateScoreViews();
                    playSoundEffect();
                }
            } else if (side == Server.SIDE_B) {
                if (countDownTimerWithPause.timeLeft() != 0) {
                    score.increaseSideB();
                    updateScoreViews();
                    playSoundEffect();
                }
            } else if (intent.getIntExtra(Server.RESET, -1) == Server.RESET_COMMAND) {
                score.reset();
                updateScoreViews();
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        ipAddressTextView.setText(getIpAddress());

        // Set the hardware buttons to control the music
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
        // Load the sound
        AudioManager audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
        soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
            @Override
            public void onLoadComplete(SoundPool soundPool, int sampleId,
                                       int status) {
                soundLoaded = true;
            }
        });
        soundId = soundPool.load(this, R.raw.gol_sound, 1);

        score = new Score();
        countDownTimerWithPause = new GameCountDownTimer(GAME_TIME_DEFAULT, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onStart() {
        super.onStart();
        LocalBroadcastManager.getInstance(this)
                .registerReceiver(dataReceiver, new IntentFilter(Server.INTENT_FILTER_ACTION));
    }

    @Override
    public void onResume() {
        super.onResume();
        try {
            server = new Server(this);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (server != null) {
            server.stop();
            server = null;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(dataReceiver);
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.game_timer_text_view)
    void onClickTimerView() {
        if (countDownTimerWithPause.isRunning()) {
            System.out.println("time remain = " + countDownTimerWithPause.timePassed() + " " + countDownTimerWithPause.timeLeft());
            countDownTimerWithPause.pause();
        } else {
            System.out.println("time remain = " + countDownTimerWithPause.timePassed() + " " + countDownTimerWithPause.timeLeft());
            countDownTimerWithPause.resume();
        }
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.start_game_button)
    void onClickStartGame() {
        score.reset();
        updateScoreViews();
        countDownTimerWithPause.cancel();
        countDownTimerWithPause.create();
        countDownTimerWithPause.resume();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.increase_score_a_button)
    void onClickIncreaseScoreA() {
        score.increaseSideA();
        updateScoreViews();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.increase_score_b_button)
    void onClickIncreaseScoreB() {
        score.increaseSideB();
        updateScoreViews();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.decrease_score_a_button)
    void onClickDecreaseScoreA() {
        score.decreaseSideA();
        updateScoreViews();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.decrease_score_b_button)
    void onClickDecreaseScoreB() {
        score.decreaseSideB();
        updateScoreViews();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.set_timer_button)
    void onClickSetTimer() {
        RadialTimePickerDialogFragment pickerFragment = RadialTimePickerDialogFragment
                .newInstance(new RadialTimePickerDialogFragment.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(RadialTimePickerDialogFragment dialog, int hourOfDay, int minute) {
                        long time = hourOfDay * 60 * 1000 + minute * 1000;
                        countDownTimerWithPause = new GameCountDownTimer(time, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
                    }
                }, 15, 12, true);
        pickerFragment.setThemeCustom(R.style.MyCustomBetterPickersRadialTimePickerDialog);
        pickerFragment.show(getSupportFragmentManager(), "picker");
    }

    private String getIpAddress() {
        WifiManager wifiMgr = (WifiManager) getSystemService(WIFI_SERVICE);
        int ip = wifiMgr.getConnectionInfo().getIpAddress();
        return Formatter.formatIpAddress(ip);
    }

    private void updateTimerView(long millisUntilFinished) {
        System.out.println(String.valueOf(millisUntilFinished) + " " + millisUntilFinished / (60 * UPDATE_INTERVAL) + " " + (millisUntilFinished / UPDATE_INTERVAL) % 60);
        long minutes = millisUntilFinished / (60 * UPDATE_INTERVAL);
        long seconds = (millisUntilFinished / UPDATE_INTERVAL) % 60;
        gameTimerTextView.setText(String.format("%02d:%02d", minutes, seconds));
    }

    private void updateScoreViews() {
        scoreATextView.setText(String.valueOf(score.getSideA()));
        scoreBTextView.setText(String.valueOf(score.getSideB()));
    }

    private void playSoundEffect() {
        AudioManager audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        float actualVolume = (float) audioManager
                .getStreamVolume(AudioManager.STREAM_MUSIC);
        float maxVolume = (float) audioManager
                .getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        float volume = actualVolume / maxVolume;
        // Is the sound already?
        if (soundLoaded) {
            soundPool.play(soundId, volume, volume, 1, 0, 1f);
            Log.i("Test", "Played sound");
        }
    }


    private class GameCountDownTimer extends CountDownTimerWithPause {
        /**
         * @param millisOnTimer
         * @param countDownInterval The interval in millis at which to execute
         *                          onTick(millisUntilFinished)} callbacks
         * @param runAtStart        True if timer should start running, false if not
         */
        public GameCountDownTimer(long millisOnTimer, long countDownInterval, boolean runAtStart) {
            super(millisOnTimer, countDownInterval, runAtStart);
        }

        @Override
        public void onTick(long millisUntilFinished) {
            updateTimerView(millisUntilFinished);
        }

        @Override
        public void onFinish() {
            Toast.makeText(MainActivity.this, "Game is over!", Toast.LENGTH_LONG).show();
        }
    }

}
