package is.handsome.foosballscoreboard;

import android.content.Context;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.codetroopers.betterpickers.radialtimepicker.RadialTimePickerDialogFragment;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class ScoreboardActivity extends AppCompatActivity {

    private static final String TAG = ScoreboardActivity.class.getSimpleName();

    private static final long GAME_TIME_DEFAULT = 5 * 60 * 1000 + 12 * 1000;
    private static final int UPDATE_INTERVAL = 1000;
    private static final boolean NOT_RUN_AFTER_CREATION = false;
    private static final int SIDE_A = 48;
    private static final int SIDE_B = 49;
    private static final int BAUD_RATE = 115200;

    @Bind(R.id.scoreboard_game_timer_text_view) TextView gameTimerTextView;
    @Bind(R.id.scoreboard_side_a_scoreboard_double_view) ScoreboardDoubleView scoreboardADoubleView;
    @Bind(R.id.scoreboard_side_b_scoreboard_double_view) ScoreboardDoubleView scoreboardBDoubleView;

    private int previousScoreA;
    private int previousScoreB;

    private SoundPool soundPool;
    private boolean crowdScreamSoundLoaded;
    private boolean whistleLoaded;
    private int crowdScreamSoundId;
    private int whistleSoundId;

    private CountDownTimerWithPause countDownTimerWithPause;
    private long currentTimeLeft;

    private static UsbSerialPort usbSerialPort;
    private final ExecutorService executor = Executors.newSingleThreadExecutor();

    private SerialInputOutputManager serialIoManager;

    private final SerialInputOutputManager.Listener serialInputListener = new SerialInputOutputManager.Listener() {

        @Override
        public void onRunError(Exception e) {
            Log.d(TAG, "Runner stopped.");
        }

        @Override
        public void onNewData(final byte[] data) {
            ScoreboardActivity.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ScoreboardActivity.this.updateReceivedData(data);
                }
            });
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scoreboard);
        ButterKnife.bind(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // Set the hardware buttons to control the music
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
        // Load the sound
        soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
        soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
            @Override
            public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
                if (sampleId == crowdScreamSoundId) {
                    crowdScreamSoundLoaded = true;
                } else if (sampleId == whistleSoundId) {
                    whistleLoaded = true;
                }
            }
        });
        crowdScreamSoundId = soundPool.load(this, R.raw.crowd_scream_sound, 1);
        whistleSoundId = soundPool.load(this, R.raw.whistle_sound, 1);

        countDownTimerWithPause = new GameCountDownTimer(GAME_TIME_DEFAULT, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (currentTimeLeft != 0) {
            countDownTimerWithPause = new GameCountDownTimer(currentTimeLeft, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
        } else {
            countDownTimerWithPause = new GameCountDownTimer(GAME_TIME_DEFAULT, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
        }

        final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager);
        if (availableDrivers.isEmpty()) {
            Toast.makeText(this, "USB devices Not found", Toast.LENGTH_SHORT).show();
        } else {
            UsbSerialDriver driver = availableDrivers.get(0);
            usbSerialPort = driver.getPorts().get(0);

            UsbDeviceConnection connection = usbManager.openDevice(usbSerialPort.getDriver().getDevice());
            if (connection == null) {
                return;
            }

            try {
                usbSerialPort.open(connection);
                usbSerialPort.setParameters(BAUD_RATE, UsbSerialPort.DATABITS_8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
            } catch (IOException e) {
                Log.e(TAG, "Error setting up device: " + e.getMessage(), e);
                try {
                    usbSerialPort.close();
                } catch (IOException e2) {
                    Log.e(TAG, "Close serial port", e2);
                }
                usbSerialPort = null;
                return;
            }
            onDeviceStateChange();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        stopIoManager();
        if (usbSerialPort != null) {
            try {
                usbSerialPort.close();
            } catch (IOException e) {
                Log.e(TAG, "Close serial port", e);
            }
            usbSerialPort = null;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        currentTimeLeft = countDownTimerWithPause.timeLeft();
        countDownTimerWithPause.cancel();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.scoreboard_game_timer_text_view)
    void onClickTimer() {
        if (countDownTimerWithPause.timeLeft() != 0) {
            if (countDownTimerWithPause.isRunning()) {
                countDownTimerWithPause.pause();
                allowChangeScoreboard(true);
            } else {
                countDownTimerWithPause.resume();
                allowChangeScoreboard(false);
            }
        } else {
            startNewGame();
        }
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.scoreboard_logo_image_view)
    void onClickLogo() {
        if (countDownTimerWithPause.timeLeft() == 0) {
            showTimerSetting();
        } else if (countDownTimerWithPause.isPaused()) {
            refreshGame();
        }
    }

    private void updateTimerView(long millisUntilFinished) {
        long minutes = millisUntilFinished / (60 * UPDATE_INTERVAL);
        long seconds = (millisUntilFinished / UPDATE_INTERVAL) % 60;
        gameTimerTextView.setText(String.format("%02d:%02d", minutes, seconds));
    }

    private void playSoundEffect() {
        AudioManager audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        float actualVolume = (float) audioManager
                .getStreamVolume(AudioManager.STREAM_MUSIC);
        float maxVolume = (float) audioManager
                .getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        float volume = actualVolume / maxVolume;

        if ((previousScoreA == 0 & scoreboardADoubleView.getCurrentValue() == 1)
                || (previousScoreB == 0 & scoreboardBDoubleView.getCurrentValue() == 1)
                || scoreboardADoubleView.getCurrentValue() == scoreboardBDoubleView.getCurrentValue()
                || Math.abs(scoreboardADoubleView.getCurrentValue() - scoreboardBDoubleView.getCurrentValue()) == 1) {
            if (crowdScreamSoundLoaded) {
                soundPool.play(crowdScreamSoundId, volume, volume, 1, 0, 1f);
            }
        } else {
            if (whistleLoaded) {
                soundPool.play(whistleSoundId, volume, volume, 1, 0, 1f);
            }
        }
    }

    private void updatePreviousScores() {
        previousScoreA = scoreboardADoubleView.getCurrentValue();
        previousScoreB = scoreboardBDoubleView.getCurrentValue();
    }

    private void startNewGame() {
        scoreboardADoubleView.reset();
        scoreboardBDoubleView.reset();

        countDownTimerWithPause.cancel();
        countDownTimerWithPause.create();
        countDownTimerWithPause.resume();

        allowChangeScoreboard(false);
    }

    private void refreshGame() {
        countDownTimerWithPause.cancel();
        countDownTimerWithPause.reset();

        scoreboardADoubleView.reset();
        scoreboardBDoubleView.reset();
        gameTimerTextView.setText(R.string.start_label);
    }

    private void allowChangeScoreboard(boolean allowed) {
        scoreboardADoubleView.setEnabled(allowed);
        scoreboardBDoubleView.setEnabled(allowed);
    }

    private void showTimerSetting() {
        if (getSupportFragmentManager().findFragmentByTag("picker") == null) {
            RadialTimePickerDialogFragment pickerFragment = RadialTimePickerDialogFragment
                    .newInstance(new RadialTimePickerDialogFragment.OnTimeSetListener() {
                        @Override
                        public void onTimeSet(RadialTimePickerDialogFragment dialog, int hourOfDay, int minute) {
                            long time = hourOfDay * 60 * 1000 + minute * 1000;
                            countDownTimerWithPause.cancel();
                            countDownTimerWithPause = new GameCountDownTimer(time, UPDATE_INTERVAL, NOT_RUN_AFTER_CREATION);
                        }
                    }, 5, 12, true);
            pickerFragment.setThemeCustom(R.style.MyCustomBetterPickersRadialTimePickerDialog);
            pickerFragment.show(getSupportFragmentManager(), "picker");
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
            gameTimerTextView.setText(R.string.start_label);
            allowChangeScoreboard(true);
        }
    }

    private void updateReceivedData(byte[] data) {
        int side = data[0];

        if (countDownTimerWithPause.isRunning()) {
            if (side == SIDE_A) {
                scoreboardBDoubleView.next();
                playSoundEffect();
            } else if (side == SIDE_B) {
                scoreboardADoubleView.next();
                playSoundEffect();
            }
            updatePreviousScores();
        }
    }

    private void stopIoManager() {
        if (serialIoManager != null) {
            Log.i(TAG, "Stopping io manager ..");
            serialIoManager.stop();
            serialIoManager = null;
        }
    }

    private void startIoManager() {
        if (usbSerialPort != null) {
            Log.i(TAG, "Starting io manager ..");
            serialIoManager = new SerialInputOutputManager(usbSerialPort, serialInputListener);
            executor.submit(serialIoManager);
        }
    }

    private void onDeviceStateChange() {
        stopIoManager();
        startIoManager();
    }

}
