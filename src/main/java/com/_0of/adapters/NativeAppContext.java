package com._0of.adapters;

/**
 * @author 0of
 */
public class NativeAppContext extends JContext{
    private static NativeAppContext gInstance = null;
    private static native long getFromNative();

    static {
        gInstance = new NativeAppContext(getFromNative());
    }

    public static NativeAppContext getInstance() {
        return gInstance;
    }

    private NativeAppContext(long handle) {
        super(handle);
    }

    public void start() {
        if (0 != _start(_handle)) {
            // something wrong
        }
    }

    /**
     * called when application is about to quit
     * (NOT request to quit)
     */
    public void quit() {
        _quit(_handle);
    }

    @Override
    protected void freeHandler() {
        super.freeHandler();
    }

    private native int _start(long handler);
    private native int _quit(long handler);
}
