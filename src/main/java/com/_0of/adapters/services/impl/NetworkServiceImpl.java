package com._0of.adapters.services.impl;

import android.os.Handler;
import android.os.Looper;

import com._0of.adapters.JObject;
import com._0of.adapters.services.NetworkService;

import java.net.MalformedURLException;
import java.net.URL;

/**
 * ONLY allow to create in UI thread
 *
 * @author 0of
 */
class NetworkServiceImpl extends JObject implements NetworkService {

    public static NetworkService acquireInstance(long handle) {
        return new NetworkServiceImpl(handle);
    }

    private Handler _postHandler;

    public NetworkServiceImpl(long handle) {
        super(handle);

        // UI thread instantiation constraint
        if (Looper.myLooper() != Looper.getMainLooper()) {
            throw new RuntimeException("Network service shall be instantiated in main thread");
        }

        // init handler
        _postHandler = new Handler();
    }

    /**
     * expose to native
     */
    class NativeEstablishProxyListener implements EstablishListener {
        private EstablishListener _original;

        public NativeEstablishProxyListener(EstablishListener _original) {
            this._original = _original;
        }

        @Override
        public void OnEstablished() {
            // post to handler's thread
            _postHandler.post(new Runnable() {
                @Override
                public void run() {
                    _original.OnEstablished();
                }
            });
        }

        @Override
        public void OnError(final int errorCode) {
            _postHandler.post(new Runnable() {
                @Override
                public void run() {
                    _original.OnError(errorCode);
                }
            });
        }

        @Override
        public void OnTimeout(final int timespan) {
            _postHandler.post(new Runnable() {
                @Override
                public void run() {
                    _original.OnTimeout(timespan);
                }
            });
        }
    }

    @Override
    public void request(String connectString, EstablishListener listener) throws MalformedURLException {
        requestTimeout(connectString, listener, -1);
    }

    @Override
    public void requestTimeout(String connectString, EstablishListener listener, int timeoutMs) throws MalformedURLException {
        URL connectURL = new URL(connectString);

        // check TCP/UDP protocol
        String protocol = connectURL.getProtocol();
        boolean isTCP = true;

        if (protocol.equalsIgnoreCase("udp")) {
            isTCP = false;
        }
        else if (!protocol.equalsIgnoreCase("tcp")) {
            // invalid protocol
            throw new MalformedURLException("Unknown protocol");
        }

        int connectID = connectString.hashCode();

        EstablishListener proxy = new NativeEstablishProxyListener(listener);
        // pass the proxy to native callings
        int nativeReturn = _requestConnect(_handle, proxy, connectURL.getHost(), connectURL.getPort(), connectID, -1 /* no timeout */, isTCP, true);
        if (0 != nativeReturn) {
            // TODO native return conversion

        }
    }

    @Override
    public void request(String connectString, final DestroyListener listener) {
        int connectID = connectString.hashCode();

        int nativeReturn = _requestDestroy(_handle, new DestroyListener() {

            @Override
            public void OnDestroyed() {
                _postHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        listener.OnDestroyed();
                    }
                });
            }

            @Override
            public void OnError(final int errorCode) {
                _postHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        listener.OnError(errorCode);
                    }
                });
            }
        }, connectID);

        if (0 != nativeReturn) {
            // TODO native return conversion

        }
    }

    @Override
    protected void freeHandler() {
        _freeHandler(_handle);
    }

    private native void _freeHandler(long handle);

    private native int _requestConnect(long handle, EstablishListener listener, String host, int port, int connID, int timeout, boolean isTCP /* false is UDP*/, boolean isIPv4);
    private native int _requestDestroy(long handle, DestroyListener listener, int connID);
}
