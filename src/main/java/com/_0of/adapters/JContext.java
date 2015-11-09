package com._0of.adapters;

/**
 * Java context adapter for Context
 *
 * @author 0of
 */
public class JContext extends JObject {
    public JContext(long handle) {
        super(handle);
    }

    public <T> T provides(Class<T> type) throws ServiceNotAvailableException {
        if (!type.isInterface()) {
            throw new RuntimeException("given type is not an interface");
        }

        Object service = null;
        try {
            service = _instantiateService(type.getName(), _handle);
        }
        catch (Exception any) {
            // suppress any exceptions
            throw new ServiceNotAvailableException();
        }

        // result may still be null
        if (service == null) {
            throw new ServiceNotAvailableException();
        }

        return (T)service;
    }

    @Override
    protected void freeHandler() {
        _freeHandler(_handle);
    }

    private native void _freeHandler(long handle);
    private native Object _instantiateService(String classPath, long handle);
}
