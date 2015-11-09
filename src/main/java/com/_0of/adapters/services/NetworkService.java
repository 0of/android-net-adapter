package com._0of.adapters.services;

import java.net.MalformedURLException;

/**
 * Network service for access via socket
 *
 * @author 0of
 */
public interface NetworkService {

    /**
     * Establishing network channel listener
     * @implNote listener will be invoked in request thread
     */
    interface EstablishListener {
        /**
         * @implSpec invoked when connected for TCP connection, whereas socket created for UDP
         */
        void OnEstablished();

        void OnError(int errorCode);

        /**
         * Only if invoked when you've provided timeout argument
         * if not given, any timeout will transform to error {@see #OnError}
         */
        void OnTimeout(int timespan);
    }

    /**
     * Destroying network channel listener
     * @implNote listener will be invoked in request thread
     */
    interface DestroyListener {
        void OnDestroyed();

        /**
         * the socket resource will be collected even exception occurred
         * @param errorCode
         */
        void OnError(int errorCode);
    }

    /**
     * request establish connection
     * @param connectString
     *  url format
     *  for TCP connection, tcp://host:port/your_customized_id
     *  for UDP connection, udp://host:port/your_customized_id
     *      typically UDP is connectionless, however abstract the those methods in order to keep API unity
     * @param listener
     */
    void request(String connectString, EstablishListener listener) throws MalformedURLException;

    /**
     * request establish connection with timeout
     * @param connectString
     *  {@see #request()}
     * @param listener
     * @param timeoutMs in millisecond
     */
    void requestTimeout(String connectString, EstablishListener listener, int timeoutMs) throws MalformedURLException;

    /**
     * request close the socket
     * @param connectString
     * @param listener
     */
    void request(String connectString, DestroyListener listener);
}
