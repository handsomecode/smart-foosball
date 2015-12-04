package is.handsome.foosballserver;

import android.content.Context;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
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

public class MainActivity extends AppCompatActivity {

    private static final long GAME_TIME_DEFAULT = 5 * 60 * 1000 + 12 * 1000;
    private static final int UPDATE_INTERVAL = 1000;
    private static final boolean NOT_RUN_AFTER_CREATION = false;
    private static final int SIDE_A = 48;
    private static final int SIDE_B = 49;

    @Bind(R.id.game_timer_text_view) TextView gameTimerTextView;
    @Bind(R.id.score_a_scoreboard_double_view) ScoreboardDoubleView scoreboardADoubleView;
    @Bind(R.id.score_b_scoreboard_double_view) ScoreboardDoubleView scoreboardBDoubleView;
//    @Bind(R.id.output_text_view) TextView usbOutputTextView;

    private SoundPool soundPool;
    boolean soundLoaded;
    private int soundId;

    private CountDownTimerWithPause countDownTimerWithPause;
    /**
     *
     */
    private static UsbSerialPort sPort;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();

    private SerialInputOutputManager mSerialIoManager;

    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {

                @Override
                public void onRunError(Exception e) {
                    Log.d("TAG", "Runner stopped.");
                }

                @Override
                public void onNewData(final byte[] data) {
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.this.updateReceivedData(data);
                        }
                    });
                }
            };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.content_scoreboard);
        ButterKnife.bind(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

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
    public void onResume() {
        super.onResume();
        final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager);
        if (availableDrivers.isEmpty()) {
            Toast.makeText(this, "USB devices Not found", Toast.LENGTH_LONG).show();
        } else {
            UsbSerialDriver driver = availableDrivers.get(0);
            sPort = driver.getPorts().get(0);

            UsbDeviceConnection connection = usbManager.openDevice(sPort.getDriver().getDevice());
            if (connection == null) {
//                usbOutputTextView.setText("Opening device failed");
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
            } catch (IOException e) {
                Log.e("TAG", "Error setting up device: " + e.getMessage(), e);
//                usbOutputTextView.setText("Error opening device: " + e.getMessage());
                try {
                    sPort.close();
                } catch (IOException e2) {
                    // Ignore.
                }
                sPort = null;
                return;
            }
//            usbOutputTextView.setText("Serial device: " + sPort.getClass().getSimpleName());
            onDeviceStateChange();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        stopIoManager();
        if (sPort != null) {
            try {
                sPort.close();
            } catch (IOException e) {
                // Ignore.
            }
            sPort = null;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        countDownTimerWithPause.cancel();
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.game_timer_text_view)
    void onClickTimerView() {
        if (countDownTimerWithPause.timeLeft() != 0) {
            if (countDownTimerWithPause.isRunning()) {
                System.out.println("time remain = " + countDownTimerWithPause.timePassed() + " " + countDownTimerWithPause.timeLeft());
                countDownTimerWithPause.pause();
                allowChangeScoreboard(true);
            } else {
                System.out.println("time remain = " + countDownTimerWithPause.timePassed() + " " + countDownTimerWithPause.timeLeft());
                countDownTimerWithPause.resume();
                allowChangeScoreboard(false);
            }
        } else {
            startNewGame();
        }
    }

    @SuppressWarnings("UnusedDeclaration") // used by ButterKnife
    @OnClick(R.id.logo_image_view)
    void onClickStartGame() {
        if (countDownTimerWithPause.timeLeft() == 0) {
            showTimerSetting();
        } else if (countDownTimerWithPause.isPaused()) {
            refreshGame();
        }
    }


    private void updateTimerView(long millisUntilFinished) {
        System.out.println(String.valueOf(millisUntilFinished) + " " + millisUntilFinished / (60 * UPDATE_INTERVAL) + " " + (millisUntilFinished / UPDATE_INTERVAL) % 60);
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
        // Is the sound already?
        if (soundLoaded) {
            soundPool.play(soundId, volume, volume, 1, 0, 1f);
            Log.i("Test", "Played sound");
        }
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

        if (side == SIDE_A) {
            scoreboardADoubleView.next();
            playSoundEffect();
        } else if (side == SIDE_B) {
            scoreboardBDoubleView.next();
            playSoundEffect();
        }
    }

    private void stopIoManager() {
        if (mSerialIoManager != null) {
            Log.i("TAG", "Stopping io manager ..");
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if (sPort != null) {
            Log.i("TAG", "Starting io manager ..");
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange() {
        stopIoManager();
        startIoManager();
    }

}
