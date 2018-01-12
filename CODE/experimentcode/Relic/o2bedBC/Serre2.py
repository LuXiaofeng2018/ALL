# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.8
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_Serre2', [dirname(__file__)])
        except ImportError:
            import _Serre2
            return _Serre2
        if fp is not None:
            try:
                _mod = imp.load_module('_Serre2', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _Serre2 = swig_import_helper()
    del swig_import_helper
else:
    import _Serre2
del version_info
try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.


def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        if _newclass:
            object.__setattr__(self, name, value)
        else:
            self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr_nondynamic(self, class_type, name, static=1):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    if (not static):
        return object.__getattr__(self, name)
    else:
        raise AttributeError(name)

def _swig_getattr(self, class_type, name):
    return _swig_getattr_nondynamic(self, class_type, name, 0)


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object:
        pass
    _newclass = 0



def mallocPy(n):
    return _Serre2.mallocPy(n)
mallocPy = _Serre2.mallocPy

def writetomem(x, i, f):
    return _Serre2.writetomem(x, i, f)
writetomem = _Serre2.writetomem

def readfrommem(x, i):
    return _Serre2.readfrommem(x, i)
readfrommem = _Serre2.readfrommem

def deallocPy(x):
    return _Serre2.deallocPy(x)
deallocPy = _Serre2.deallocPy

def minmod(a, b, c):
    return _Serre2.minmod(a, b, c)
minmod = _Serre2.minmod

def GNall(x, h, u, b, g, n, nBC, dx):
    return _Serre2.GNall(x, h, u, b, g, n, nBC, dx)
GNall = _Serre2.GNall

def conc(a, b, c, n, m, k, d):
    return _Serre2.conc(a, b, c, n, m, k, d)
conc = _Serre2.conc

def getufromG(h, G, bed, u0, u1, h0, h1, b0, b1, dx, n, ublank):
    return _Serre2.getufromG(h, G, bed, u0, u1, h0, h1, b0, b1, dx, n, ublank)
getufromG = _Serre2.getufromG

def getGfromu(h, u, bed, u0, u1, h0, h1, b0, b1, dx, n, Gblank):
    return _Serre2.getGfromu(h, u, bed, u0, u1, h0, h1, b0, b1, dx, n, Gblank)
getGfromu = _Serre2.getGfromu

def evolvewrapperiodic(G, h, bed, g, dx, dt, n, nBCn, theta, hbc, Gbc, ubc):
    return _Serre2.evolvewrapperiodic(G, h, bed, g, dx, dt, n, nBCn, theta, hbc, Gbc, ubc)
evolvewrapperiodic = _Serre2.evolvewrapperiodic

def getufromGperiodic(h, G, bed, dx, n, ublank):
    return _Serre2.getufromGperiodic(h, G, bed, dx, n, ublank)
getufromGperiodic = _Serre2.getufromGperiodic

def evolvewrapBC(G, h, bed, h0, h1, u0, u1, G0, G1, h0h, h1h, u0h, u1h, G0h, G1h, b0, b1, g, dx, dt, n, nBC, nBCn, theta, hbc, Gbc, ubc):
    return _Serre2.evolvewrapBC(G, h, bed, h0, h1, u0, u1, G0, G1, h0h, h1h, u0h, u1h, G0h, G1h, b0, b1, g, dx, dt, n, nBC, nBCn, theta, hbc, Gbc, ubc)
evolvewrapBC = _Serre2.evolvewrapBC

def evolvewrapBCSponge(G, h, bed, h0, h1, u0, u1, G0, G1, h0h, h1h, u0h, u1h, G0h, G1h, b0, b1, g, dx, dt, n, nBC, nBCn, theta, hbc, Gbc, ubc, xbc, xsl, xsu, a, hbase, max):
    return _Serre2.evolvewrapBCSponge(G, h, bed, h0, h1, u0, u1, G0, G1, h0h, h1h, u0h, u1h, G0h, G1h, b0, b1, g, dx, dt, n, nBC, nBCn, theta, hbc, Gbc, ubc, xbc, xsl, xsu, a, hbase, max)
evolvewrapBCSponge = _Serre2.evolvewrapBCSponge
# This file is compatible with both classic and new-style classes.


