package is.handsome.foosballserver;

import com.squareup.otto.Bus;

public final class BusProvider {
    private static final Bus BUS = new AndroidBus();

    public static Bus getInstance() {
        return BUS;
    }

    private BusProvider() {
        // No instances.
    }
}
