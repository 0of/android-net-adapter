package com._0of.adapters;

import android.os.Handler;

/**
 * expose to native layer
 * @author 0of
 */
public class UIMessageQueue {
    private Handler _postHandler;

    public UIMessageQueue() {
        _postHandler = new Handler();
    }

    public void post(JMessage message) {
        _postHandler.post(new UIMessageHandler(message));
    }
}
