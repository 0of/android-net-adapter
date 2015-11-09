package com._0of.adapters;

import java.text.MessageFormat;

/**
 * @author 0of
 */
public class ServiceNotAvailableException extends Exception {

    public ServiceNotAvailableException() {
        super("Service not available");
    }

    public ServiceNotAvailableException(String service) {
        super(MessageFormat.format("Service [{0}] not available", service));
    }
}
