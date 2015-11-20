package `is`.handsome.foosballscoreboard

import android.animation.TimeInterpolator
import android.content.Context
import android.support.annotation.DrawableRes
import android.util.AttributeSet
import android.view.Gravity
import android.view.ViewGroup
import android.view.animation.AccelerateInterpolator
import android.view.animation.BounceInterpolator
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.LinearLayout

public class ScoreboardItemView : LinearLayout {

    constructor(context: Context?) : this(context, null)

    constructor(context: Context?, attrs: AttributeSet?) : this(context, attrs, 0)

    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr) {
        backTopImage = ImageView(context)
        frontTopImage = ImageView(context)
        backBottomImage = ImageView(context)
        frontBottomImage = ImageView(context)
        topOverlayView = ImageView(context)
        bottomOverlayView = ImageView(context)
        init()
    }

    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int, defStyleRes: Int)
    : super(context, attrs, defStyleAttr, defStyleRes) {
        backTopImage = ImageView(context)
        frontTopImage = ImageView(context)
        backBottomImage = ImageView(context)
        frontBottomImage = ImageView(context)
        topOverlayView = ImageView(context)
        bottomOverlayView = ImageView(context)
        init()
    }

    public var currentValue: Int = 0
        set(value) {
            val oldValue = field
            field = if (value < 0 || value > 9) 0 else value
            if (field == 9 && oldValue == 0) {
                animateCountDown()
            } else if (field == 0 && oldValue == 9) {
                animateCountUp()
            } else if (field > oldValue) {
                animateCountUp()
            } else if (field < oldValue) {
                animateCountDown()
            }
        }

    private val backTopImage: ImageView
    private val backBottomImage: ImageView
    private val frontTopImage: ImageView
    private val frontBottomImage: ImageView

    private val topOverlayView: ImageView
    private val bottomOverlayView: ImageView
    private val defaultInterpolator: TimeInterpolator = AccelerateInterpolator()
    private val animationTime = 250L

    override fun onLayout(changed: Boolean, l: Int, t: Int, r: Int, b: Int) {
        super.onLayout(changed, l, t, r, b)
        if (changed) {
            frontBottomImage.translationY = (-frontBottomImage.height / 2).toFloat()
            frontBottomImage.scaleY = 0f
        }
    }

    private fun init() {
        orientation = LinearLayout.VERTICAL

        var topFrame = FrameLayout(context)
        initFrame(topFrame)
        initImageView(backTopImage, getTopImageForCurrentValue(), topFrame)
        initImageView(topOverlayView, R.drawable.top_overlay, topFrame)
        topOverlayView.alpha = 0f
        initImageView(frontTopImage, getTopImageForCurrentValue(), topFrame)
        addView(topFrame)

        var bottomFrame = FrameLayout(context)
        initFrame(bottomFrame)
        initImageView(backBottomImage, getBottomImageForCurrentValue(), bottomFrame)
        initImageView(bottomOverlayView, R.drawable.bottom_overlay, bottomFrame)
        bottomOverlayView.alpha = 0f
        initImageView(frontBottomImage, getBottomImageForCurrentValue(), bottomFrame)
        addView(bottomFrame)
    }

    private fun initFrame(frame: FrameLayout) {
        frame.layoutParams = LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 0)
        (frame.layoutParams as LinearLayout.LayoutParams).weight = 1f
    }

    private fun initImageView(image: ImageView, @DrawableRes imageRes: Int, parent: ViewGroup) {
        image.layoutParams = FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT)
        (image.layoutParams as FrameLayout.LayoutParams).gravity = Gravity.BOTTOM
        image.setImageResource(imageRes)
        parent.addView(image)
    }

    private fun animateCountUp() {
        isEnabled = false
        frontTopImage.animate().cancel()
        frontBottomImage.animate().cancel()
        bottomOverlayView.animate().cancel()
        val currentTopId = getTopImageForCurrentValue()
        val currentBottomId = getBottomImageForCurrentValue()
        backTopImage.setImageResource(currentTopId)
        frontBottomImage.setImageResource(currentBottomId)

        frontBottomImage.translationY = (-frontBottomImage.height / 2).toFloat()
        frontBottomImage.scaleY = 0f

        frontTopImage.animate().scaleY(0f)
                .translationY(frontTopImage.height.toFloat())
                .setInterpolator(defaultInterpolator)
                .setDuration(animationTime)
                .withStartAction {
                    frontBottomImage.animate()
                            .scaleY(1f)
                            .translationY(0f)
                            .setDuration(animationTime)
                            .setInterpolator(BounceInterpolator())
                            .setStartDelay(animationTime)
                            .withEndAction {
                                frontTopImage.setImageResource(currentTopId)
                                frontTopImage.scaleY = 1f
                                frontTopImage.translationY = 0f

                                backBottomImage.setImageResource(currentBottomId)
                            }.start()
                    bottomOverlayView.animate()
                            .alpha(0.8f)
                            .setStartDelay(animationTime)
                            .setDuration(animationTime)
                            .withEndAction {
                                isEnabled = true
                                bottomOverlayView.alpha = 0f
                            }
                            .start()
                }.start()
    }

    private fun animateCountDown() {
        isEnabled = false
        frontTopImage.animate().cancel()
        frontBottomImage.animate().cancel()
        topOverlayView.animate().cancel()
        val currentTopId = getTopImageForCurrentValue()
        val currentBottomId = getBottomImageForCurrentValue()
        frontTopImage.setImageResource(currentTopId)
        backBottomImage.setImageResource(currentBottomId)

        frontTopImage.translationY = (frontBottomImage.height / 2).toFloat()
        frontTopImage.scaleY = 0f

        frontBottomImage.animate()
                .scaleY(0f)
                .translationY(-frontBottomImage.height.toFloat() / 2)
                .setInterpolator(defaultInterpolator)
                .setDuration(animationTime)
                .withStartAction {
                    frontTopImage.animate()
                            .scaleY(1f)
                            .translationY(0f)
                            .setDuration(animationTime)
                            .setInterpolator(BounceInterpolator())
                            .setStartDelay(animationTime)
                            .withEndAction {
                                frontBottomImage.setImageResource(currentBottomId)
                                frontBottomImage.scaleY = 1f
                                frontBottomImage.translationY = 0f

                                backTopImage.setImageResource(currentTopId)
                            }.start()
                    topOverlayView.animate()
                            .alpha(0.8f)
                            .setStartDelay(animationTime)
                            .setDuration(animationTime)
                            .withEndAction {
                                topOverlayView.alpha = 0f
                                isEnabled = true
                            }
                            .start()
                }.start()
    }

    private fun getTopImageForCurrentValue(): Int {
        return resources.getIdentifier("top" + currentValue, "drawable", context.packageName);
    }

    private fun getBottomImageForCurrentValue(): Int {
        return resources.getIdentifier("bottom" + currentValue, "drawable", context.packageName);
    }
}