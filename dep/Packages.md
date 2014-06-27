# Packages

CMaNGOS depends on the following open source libraries.

## Shipped packages

These are the libraries that comes with CMaNGOS:

| Package | Version |
| ------- | ------- |
| [ACE (ADAPTIVE Communication Environment)](http://www.cs.wustl.edu/~schmidt/ACE.html) | 6.2.6 |
| [MySQL Connector/C](http://dev.mysql.com/downloads/connector/c/) (*) | 6.1.3 |
| [Intel Threading Building Blocks](https://www.threadingbuildingblocks.org/) (*) | 4.2 Update 5 |
| [recastnavigation (Recast is state of the art navigation mesh construction toolset for games)](https://github.com/memononen/recastnavigation) | 1.4 |
| [zlib (A Massively Spiffy Yet Delicately Unobtrusive Compression Library)](http://www.zlib.net/) (*) | 1.2.8 |
| [Artyom Beilis's Boost Backtrace Library](http://article.gmane.org/gmane.comp.lib.boost.devel/209982) | initial |
| [G3D (A commercial-grade C++ 3D engine](http://g3d.sourceforge.net/) | 8.01 |
| [bzip2 (A freely available, patent free, high-quality data compressor)]() | 1.0.5 |
| [UTF-8 CPP (A simple, portable and lightweight generic library for handling UTF-8 encoded strings.)](http://sourceforge.net/projects/utfcpp/) | 2.2.4 |
| [libMPQ (A library for reading MPQ files)](https://libmpq.org/) | Revision 300 |
| [gSOAP (Toolkit for SOAP and REST Web Services and XML-Based Applications)](http://gsoap2.sourceforge.net/) | 2.7.15 |
| Mersenne Twister (A random number generator -- A C++ class MTRand) | 1.0 |
| [Boost Extension (A library providing for the creation of plugins for C++ software)](http://sourceforge.net/projects/boost-extension/) | initial |

## Not shipped packages

The packages below must be provided for CMaNGOS in order to compile the software.

| Package | Version |
| ------- | ------- |
| [Boost (Free peer-reviewed portable C++ source libraries)](http://www.boost.org/) | 1.55.0 |
| [OpenSSL (The Open Source toolkit for SSL/TLS)](http://www.openssl.org/)| 1.0.1h |
| [PostgreSQL (The world's most advanced open source database)](http://www.postgresql.org/) (!) | 9.3.4 |

### Note

(*) marks Windows only libraries. On Unix/Linux based operating systems you will need
to install them manually.

(!) marks optional libraries which will be required if you enable a associated core feature.
