package `is`.handsome.foosballserver

import android.content.Context
import android.graphics.PointF
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.ViewConfiguration
import android.view.ViewGroup
import android.widget.LinearLayout

public class ScoreboardDoubleView : LinearLayout {

    constructor(context: Context?) : this(context, null)

    constructor(context: Context?, attrs: AttributeSet?) : this(context, attrs, 0)

    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr) {
        firstDigit = ScoreboardItemView(context)
        secondDigit = ScoreboardItemView(context)
        touchSlop = ViewConfiguration.get(context).scaledTouchSlop * 5
        init()
    }

    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int, defStyleRes: Int)
    : super(context, attrs, defStyleAttr, defStyleRes) {
        firstDigit = ScoreboardItemView(context)
        secondDigit = ScoreboardItemView(context)
        touchSlop = ViewConfiguration.get(context).scaledTouchSlop * 5
        init()
    }

    private var firstDigit: ScoreboardItemView
    private var secondDigit: ScoreboardItemView

    private var firstTap: PointF? = null
    private var touchSlop: Int

    public var currentValue: Int = 0
        set(value) {
            field = if (value > 0) value % 100 else 0
            animateSwitch()
        }

    override fun dispatchTouchEvent(ev: MotionEvent?): Boolean {
        if (isEnabled && firstDigit.isEnabled && secondDigit.isEnabled) {
            val motionEvent: MotionEvent = ev ?: return false
            when (motionEvent.action) {
                MotionEvent.ACTION_DOWN -> {
                    if (motionEvent.pointerCount > 1) return false
                    firstTap = PointF(motionEvent.rawX, motionEvent.rawY)
                    return true
                }
                MotionEvent.ACTION_UP -> {
                    if (firstTap == null) return false
                    if (firstTap!!.y - motionEvent.rawY > touchSlop) {
                        currentValue--
                    } else if (motionEvent.rawY - firstTap!!.y > touchSlop) {
                        currentValue++
                    }
                }
            }
        }
        return super.dispatchTouchEvent(ev)
    }

    public fun next() {
        currentValue++
    }

    public fun reset() {
        currentValue = 0
    }

    private fun animateSwitch() {
        firstDigit.currentValue = currentValue / 10
        secondDigit.currentValue = currentValue % 10
    }

    private fun init() {
        initItem(firstDigit)
        initItem(secondDigit)
    }

    private fun initItem(digit: ScoreboardItemView) {
        digit.layoutParams = LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT)
        (digit.layoutParams as LinearLayout.LayoutParams).weight = 1f
        addView(digit)
    }
}
