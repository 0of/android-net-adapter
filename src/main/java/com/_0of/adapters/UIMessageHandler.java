package com._0of.adapters;

/**
 * @author 0of
 */
public class UIMessageHandler implements Runnable {
    private JMessage _handleMessage;

    public UIMessageHandler(JMessage handleMessage) {
        _handleMessage = handleMessage;
    }

    @Override
    public void run() {
        _onMessageHandle(_handleMessage.takeNativeHandler());
    }

    private native void _onMessageHandle(long handle);
}
