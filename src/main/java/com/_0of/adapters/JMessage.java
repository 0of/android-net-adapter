package com._0of.adapters;

/**
 * instantiate from native
 *
 * @author 0of
 */
public class JMessage extends JObject{

    public JMessage(long handle) {
        super(handle);
    }

    /**
     *
     * @return
     */
    public long takeNativeHandler() {
        long handle = _handle;
        _handle = NULL;
        return handle;
    }

    @Override
    protected void freeHandler() {
        if (_handle != NULL) {
            // free
        }
    }
}
