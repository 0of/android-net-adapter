package com._0f.adapters;

/**
 * Adapter for JNI pointer
 *
 * @author 0of
 */
public abstract class JObject {
    protected final static long NULL = 0;

    protected long _handle;

    public JObject() {
        _handle = NULL;
    }

    public JObject(long handler) {
        _handle = handler;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        if (_handle != NULL) {
            freeHandler();
        }
    }

    abstract protected void freeHandler();
}
