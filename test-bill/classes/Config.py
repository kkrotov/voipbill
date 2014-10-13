class Config:
    def __init__(self, **kwds):
        self.__dict__.update(kwds)

    def merge(self, otherCfg):
        self.__dict__.update(otherCfg.__dict__)
        return self
