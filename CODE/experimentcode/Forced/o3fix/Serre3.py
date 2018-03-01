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
            fp, pathname, description = imp.find_module('_Serre3', [dirname(__file__)])
        except ImportError:
            import _Serre3
            return _Serre3
        if fp is not None:
            try:
                _mod = imp.load_module('_Serre3', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _Serre3 = swig_import_helper()
    del swig_import_helper
else:
    import _Serre3
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



def conc(a, b, c, n, m, k, d):
    return _Serre3.conc(a, b, c, n, m, k, d)
conc = _Serre3.conc

def mallocPy(n):
    return _Serre3.mallocPy(n)
mallocPy = _Serre3.mallocPy

def writetomem(x, i, f):
    return _Serre3.writetomem(x, i, f)
writetomem = _Serre3.writetomem

def readfrommem(x, i):
    return _Serre3.readfrommem(x, i)
readfrommem = _Serre3.readfrommem

def deallocPy(x):
    return _Serre3.deallocPy(x)
deallocPy = _Serre3.deallocPy

def TDMA(a, b, c, d, n, x):
    return _Serre3.TDMA(a, b, c, d, n, x)
TDMA = _Serre3.TDMA

def PENT(e, a, d, c, f, B, n, x):
    return _Serre3.PENT(e, a, d, c, f, B, n, x)
PENT = _Serre3.PENT

def midpt2ca(qm, qabeg, qaend, dx, n, qa):
    return _Serre3.midpt2ca(qm, qabeg, qaend, dx, n, qa)
midpt2ca = _Serre3.midpt2ca

def ca2midpt(qa, qabeg, qaend, dx, n, qm):
    return _Serre3.ca2midpt(qa, qabeg, qaend, dx, n, qm)
ca2midpt = _Serre3.ca2midpt

def ufromGh(G, h, hbeg, hend, ubeg, uend, dx, n, nBC, u):
    return _Serre3.ufromGh(G, h, hbeg, hend, ubeg, uend, dx, n, nBC, u)
ufromGh = _Serre3.ufromGh

def Gfromuh(u, h, hbeg, hend, ubeg, uend, dx, n, nBC, G):
    return _Serre3.Gfromuh(u, h, hbeg, hend, ubeg, uend, dx, n, nBC, G)
Gfromuh = _Serre3.Gfromuh

def phikm(r):
    return _Serre3.phikm(r)
phikm = _Serre3.phikm

def phikp(r):
    return _Serre3.phikp(r)
phikp = _Serre3.phikp

def weightsum(a, x, b, y, n, z):
    return _Serre3.weightsum(a, x, b, y, n, z)
weightsum = _Serre3.weightsum

def evolvewrap(Ga, ha, Gabeg, Gaend, habeg, haend, hmbeg, hmend, uabeg, uaend, umbeg, umend, Gabeg1, Gaend1, habeg1, haend1, hmbeg1, hmend1, uabeg1, uaend1, umbeg1, umend1, Gabeg2, Gaend2, habeg2, haend2, hmbeg2, hmend2, uabeg2, uaend2, umbeg2, umend2, nfcBC, nGsBC, g, dx, dt, n, nBCa, nBCm, t, x):
    return _Serre3.evolvewrap(Ga, ha, Gabeg, Gaend, habeg, haend, hmbeg, hmend, uabeg, uaend, umbeg, umend, Gabeg1, Gaend1, habeg1, haend1, hmbeg1, hmend1, uabeg1, uaend1, umbeg1, umend1, Gabeg2, Gaend2, habeg2, haend2, hmbeg2, hmend2, uabeg2, uaend2, umbeg2, umend2, nfcBC, nGsBC, g, dx, dt, n, nBCa, nBCm, t, x)
evolvewrap = _Serre3.evolvewrap

def HankEnergyall(x, h, u, g, n, nBC, dx):
    return _Serre3.HankEnergyall(x, h, u, g, n, nBC, dx)
HankEnergyall = _Serre3.HankEnergyall
# This file is compatible with both classic and new-style classes.

