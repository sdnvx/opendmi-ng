# Third-party copyright

## LibYAML

Copyright (c) 2017-2020 Ingy döt Net\
Copyright (c) 2006-2016 Kirill Simonov

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## SMBIOS data corpus

The `data` directory holds dumps of the SMBIOS tables of real and virtual
systems, which are used as test data and are not part of the software built
from this repository. Some of the dumps come from other projects, which are
listed below along with their licenses. The source and the license of each of
these dumps are also recorded in the `00-index.yml` file of its vendor
directory.

The dumps are kept as they were published, and may carry the serial numbers,
UUIDs and other identifiers of the systems they come from.

### u-root

Source: <https://github.com/u-root/u-root>, commit `8216e699c2b6`

- `data/asus/ux305la.bin`
- `data/gigabyte/ga-ma74gmt-s2.bin`
- `data/gigabyte/x399-aorus-xtreme.bin`
- `data/lenovo/thinkpad-t480s-20l8.bin`
- `data/lenovo/thinkpad-t490-20n2.bin`
- `data/lenovo/thinkpad-w510-4319.bin`
- `data/msi/ms-7816.bin`
- `data/supermicro/x9dbl-if.bin`
- `data/synology/rs3614xs-plus.bin`
- `data/toshiba/satellite-pro-l70-a.bin`
- `data/vmware/virtual-platform-2017.bin`

BSD 3-Clause License

Copyright (c) 2012-2019, u-root Authors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### Framework System

Source: <https://github.com/FrameworkComputer/framework-system>, commit `9c0f6cf83ec3`

- `data/framework/laptop-12-13th-gen-intel.bin`
- `data/framework/laptop-13-12th-gen-intel.bin`
- `data/framework/laptop-13-13th-gen-intel.bin`
- `data/framework/laptop-13-intel-core-ultra-1.bin`

BSD 3-Clause License

Copyright (c) 2023, Framework Computer Inc

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### Gigabyte Z490 Vision G Hackintosh OpenCore

Source: <https://github.com/5T33Z0/Gigabyte-Z490-Vision-G-Hackintosh-OpenCore>, commit `ab49469652c4`

- `data/gigabyte/z490-vision-g.bin`

BSD 3-Clause License

Copyright (c) 2022, 5T33Z0

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### dmidecode (Rust crate)

Source: <https://github.com/jcreekmore/dmidecode>, commit `742bc577a851`

- `data/dell/poweredge-r777sd.bin`
- `data/dell/precision-tower-3620.bin`
- `data/lenovo/thinkpad-t430-2347.bin`
- `data/supermicro/x10dal-i.bin`

MIT License

Copyright (c) 2023 Jonathan Creekmore

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### lazybios

Source: <https://github.com/LazySeldi/lazybios>, commit `03d3ac28c215`

- `data/microsoft/hyper-v-gen1-1.bin`
- `data/microsoft/hyper-v-gen2-uefi-4.1.bin`
- `data/qemu/pc-i440fx-9.2.bin`
- `data/veertu/macmini6-2.bin`

MIT License

Copyright (c) 2024-2026 lazybios contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### smbios-lib

Source: <https://github.com/jrgerber/smbios-lib>, commit `6b024781c155`

- `data/microsoft/surface-laptop-3.bin`

MIT License

Copyright (c) 2021 jrgerber

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### Intel NUC10 Hackintosh

Source: <https://github.com/Lorys89/Intel-NUC10-Hackintosh>, commit `46bde9e19e02`

- `data/intel/nuc10i5fnh.bin`

MIT License

Copyright (c) 2023 Lorys89

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### Intel NUC8 Hackintosh

Source: <https://github.com/Lorys89/Intel-NUC8-Hackintosh>, commit `a25496c202b5`

- `data/intel/nuc8i3beh.bin`

MIT License

Copyright (c) 2023 Lorys89

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON IN FRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### HP EliteBook 830 G6 OSX86

Source: <https://github.com/VGerris/HP-Elitebook-830-G6-OSX86>, commit `ac3f1d42ecbb`

- `data/hp/elitebook-830-g6-4we10av.bin`

Licensed under the Apache License, Version 2.0, see
<https://github.com/VGerris/HP-Elitebook-830-G6-OSX86/blob/ac3f1d42ecbb936a0a6df30ae8f3f6d8372e2918/LICENSE>.

### InfraSIM Compute

Source: <https://github.com/InfraSIM/infrasim-compute>, commit `d8d81dc278a5`

- `data/dell/poweredge-c6320.bin`
- `data/dell/poweredge-r630-2.bin`
- `data/dell/poweredge-r640.bin`
- `data/dell/poweredge-r730.bin`
- `data/dell/poweredge-r730xd.bin`
- `data/dell/poweredge-r740xd.bin`
- `data/intel/s2600kp.bin`
- `data/intel/s2600tp.bin`
- `data/intel/s2600wtt.bin`
- `data/quanta/d51b-2u-dual-10g-lom.bin`
- `data/quanta/quantaplex-t41s-2u.bin`

Copyright (c) EMC Corporation. The repository has no license file, and its
source files are licensed under the Apache License, Version 2.0, see
<https://www.apache.org/licenses/LICENSE-2.0>. Some of the dumps were edited
by the project for simulation.

### libsmbios

Source: <https://github.com/dell/libsmbios>, commits `59d5df9ea2de`, `7ae0f3cb8f2d`, `f01a21763180`

- `data/cisco/ids-4235.bin`
- `data/dell/dimension-4400.bin`
- `data/dell/latitude-d500.bin`
- `data/dell/latitude-d610.bin`
- `data/dell/optiplex-gx110.bin`
- `data/dell/powerapp-web-110.bin`
- `data/dell/poweredge-1300-600.bin`
- `data/dell/poweredge-1650.bin`
- `data/dell/poweredge-1655mc-1.bin`
- `data/dell/poweredge-1655mc-2.bin`
- `data/dell/poweredge-1750-1.bin`
- `data/dell/poweredge-1750-2.bin`
- `data/dell/poweredge-1850.bin`
- `data/dell/poweredge-2450.bin`
- `data/dell/poweredge-2500.bin`
- `data/dell/poweredge-2600.bin`
- `data/dell/poweredge-2650.bin`
- `data/dell/poweredge-400sc.bin`
- `data/dell/poweredge-4400.bin`
- `data/dell/poweredge-4600.bin`
- `data/dell/poweredge-500sc.bin`
- `data/dell/poweredge-600sc.bin`
- `data/dell/poweredge-6450-550.bin`
- `data/dell/poweredge-650.bin`
- `data/dell/poweredge-6600.bin`
- `data/dell/poweredge-8450.bin`
- `data/dell/poweredge-sc420.bin`
- `data/dell/powervault-755n.bin`
- `data/dell/precision-t7600.bin`
- `data/dell/precision-workstation-390.bin`
- `data/dell/xps-13-9365.bin`
- `data/unisys/es3020.bin`
- `data/unisys/es3040.bin`
- `data/unisys/es3120l.bin`

Dual-licensed under the Open Software License, version 2.1, or the GNU General
Public License, version 2 or later, see
<https://github.com/dell/libsmbios/blob/59d5df9ea2deee3b03c02c68a5026733b0f193ba/COPYING>.

### go-smbios

Source: <https://github.com/siderolabs/go-smbios>, commit `063f5dc16e0a`

- `data/asrock/x570d4u.bin`
- `data/beelink/eq12.bin`
- `data/dell/poweredge-r630.bin`
- `data/minisforum/um790-pro.bin`
- `data/supermicro/h8qg6.bin`
- `data/supermicro/sys-1027r-wrf.bin`

Licensed under the Mozilla Public License, version 2.0, see
<https://github.com/siderolabs/go-smbios/blob/063f5dc16e0a86d4dbb8290d197bdb134d9bafac/LICENSE>.

### truenas_pydmi

Source: <https://github.com/truenas/truenas_pydmi>, commit `b4cd7401e53d`

- `data/ixsystems/freenas-mini-3.0-x.bin`
- `data/ixsystems/truenas-f60-ha.bin`
- `data/ixsystems/truenas-h30-ha.bin`
- `data/ixsystems/truenas-m50-ha.bin`
- `data/qemu/pc-q35-10.0.bin`

Licensed under the GNU Lesser General Public License, version 3, see
<https://github.com/truenas/truenas_pydmi/blob/b4cd7401e53d9fa7ebebf7641b6239dbc2f8ef6d/LICENSE>.

### Dell Inspiron 5370 Hackintosh

Source: <https://github.com/dreamwhite/dell-inspiron-5370-hackintosh>, commit `e8ea4c587cb9`

- `data/dell/inspiron-5370.bin`

Licensed under the GNU General Public License, version 3, see
<https://github.com/dreamwhite/dell-inspiron-5370-hackintosh/blob/e8ea4c587cb90c796f8c33cd00214afb026353c1/LICENSE>.

### Fujitsu Esprimo Q958 Hackintosh OpenCore

Source: <https://github.com/5T33Z0/Fujitsu-Esprimo-Q958-Hackintosh-OpenCore>, commit `29650d395af6`

- `data/fujitsu/esprimo-q958.bin`

Licensed under the GNU General Public License, version 3, see
<https://github.com/5T33Z0/Fujitsu-Esprimo-Q958-Hackintosh-OpenCore/blob/29650d395af6bff9e4f61d54a9c61711c6d92ae1/LICENSE>.

### python-dmidecode

Source: <https://github.com/nima/python-dmidecode>, commit `4fdb678f9bea`

- `data/hp/proliant-dl585-g2-413930-371.bin`
- `data/parallels/virtual-platform-3.0.bin`
- `data/qemu/kvm-bios-2007.bin`
- `data/vmware/virtual-platform-2008.bin`

Licensed under the GNU General Public License, version 2, see
<https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

### Sources without a license

The following repositories publish the dumps with no license. Most of them are
the system reports of OpenCore, which hold the SMBIOS tables of the firmware of
the systems. The dumps are kept here as test data only.

- <https://github.com/Baio1977/Hackintosh-EFI-Collection>, commit `4b71423452d9`:
  - `data/acer/aspire-a515-51g.bin`
  - `data/acer/aspire-a515-52g.bin`
  - `data/acer/nitro-an515-52.bin`
  - `data/acer/predator-g9-592.bin`
  - `data/alldocube/i1402a.bin`
  - `data/asus/gl753vd.bin`
  - `data/asus/n551jx.bin`
  - `data/asus/rog-strix-z690-a-gaming-wifi-d4.bin`
  - `data/asus/x756uam.bin`
  - `data/chuwi/corebook-x.bin`
  - `data/dell/latitude-5400.bin`
  - `data/dell/latitude-7389.bin`
  - `data/dell/latitude-7390.bin`
  - `data/dell/latitude-7490.bin`
  - `data/dell/latitude-e7450.bin`
  - `data/dell/precision-m3800.bin`
  - `data/dell/vostro-5490.bin`
  - `data/dell/xps-13-9300.bin`
  - `data/dell/xps-13-9350.bin`
  - `data/gigabyte/h610m-h-v2-ddr4.bin`
  - `data/hp/250-g7-6bp85ea.bin`
  - `data/hp/250-g8-27k26ea.bin`
  - `data/hp/elitebook-820-g2-f6n30av.bin`
  - `data/hp/elitebook-840-g1-d1f44av.bin`
  - `data/hp/elitebook-revolve-810-g2-j0z58av.bin`
  - `data/hp/laptop-15-da0xxx-4re55ea.bin`
  - `data/hp/notebook-x0l36ea.bin`
  - `data/hp/omen-laptop-15-ek0xxx-3h851ea.bin`
  - `data/hp/pavilion-x360-convertible-14-cd0xxx-4ls25pa.bin`
  - `data/hp/probook-450-g2-k9k29ea.bin`
  - `data/hp/spectre-pro-x360-g2-v1b01ea.bin`
  - `data/huawei/nblb-wax9n.bin`
  - `data/lenovo/ideapad-330s-15ikb-81f5.bin`
  - `data/lenovo/ideapad-l340-15irh-gaming-81lk.bin`
  - `data/lenovo/legion-5-15imh05h-81y6.bin`
  - `data/lenovo/thinkpad-l15-gen-1-20u4.bin`
  - `data/lenovo/thinkpad-l420-7854.bin`
  - `data/lenovo/thinkpad-x131e-3367.bin`
  - `data/lenovo/thinkpad-x240-20am.bin`
  - `data/lenovo/yoga-530-14ikb-81ek.bin`
  - `data/lenovo/yoga-c640-13iml-81ue.bin`
  - `data/lenovo/yoga-slim-7-14iil05-82a1.bin`
  - `data/xiaomi/mi-gaming-laptop-15.6.bin`
  - `data/xiaomi/tm1701.bin`
- <https://github.com/Edwardwich/ACER-A515-52-Hackintosh>, commit `7bcd789da6ca`:
  - `data/acer/aspire-a515-52.bin`
- <https://github.com/5T33Z0/iMac-2010-Big-Sur>, commit `ee3c333fe53e`:
  - `data/apple/imac11-3.bin`
- <https://github.com/Baio1977/Asus-ROG-GL703ge>, commit `8fe4c05accaa`:
  - `data/asus/strix-17-gl703ge.bin`
- <https://github.com/Baio1977/Asus-Vivobook-Pro-N580VD>, commit `81b9d5e7c70d`:
  - `data/asus/x580vd.bin`
- <https://github.com/Baio1977/Dell-Latitude-5280>, commit `d956c272d712`:
  - `data/dell/latitude-5280.bin`
- <https://github.com/Lorys89/DELL_OPTIPLEX_9030_AIO>, commit `aef8f6f2159d`:
  - `data/dell/optiplex-9030-aio.bin`
- <https://github.com/ByteMatrix123/Gigabyte-B460-AORUS-PRO-AC-Hackintosh>, commit `5ba316eb6b35`:
  - `data/gigabyte/b460-aorus-pro-ac.bin`
- <https://github.com/tohas1986/smbios_parser>, commit `88cefc5a5993`:
  - `data/gigabyte/noname-server-f15.bin`
- <https://github.com/Baio1977/GA-Z390M-Gaming>, commit `a714418dd1e1`:
  - `data/gigabyte/z390-m-gaming.bin`
- <https://github.com/Baio1977/GA-Z490M-Gaming-X>, commit `b999518d2ff5`:
  - `data/gigabyte/z490m-gaming-x.bin`
- <https://github.com/Baio1977/HP-250-G6-Kabylake>, commit `b553bf011809`:
  - `data/hp/250-g6-3qm22ea.bin`
- <https://github.com/Baio1977/HP630-CPU-Arrandale>, commit `fa72784850d1`:
  - `data/hp/630-a6e72ea.bin`
- <https://github.com/Edwardwich/hp-DA1023nia-macOS>, commit `818132e0a132`:
  - `data/hp/laptop-15-da1xxx-6rq05ea.bin`
- <https://github.com/Baio1977/HP16-Pavilion-Gaming>, commit `3d8f52c8adb1`:
  - `data/hp/pavilion-gaming-laptop-16-a0xxx-2z2e5ea.bin`
- <https://github.com/BimaBizz/Master-OC-HP-Probook-4430s>, commit `2b45f6dee8b9`:
  - `data/hp/probook-4430s-lx014pa.bin`
- <https://github.com/Baio1977/Intel-NUC8i7HN>, commit `516da10d3508`:
  - `data/intel/nuc8i7hvk.bin`
- <https://github.com/Baio1977/Lenovo-IdeaPad-L340-Gaming>, commit `8ebb50f777ca`:
  - `data/lenovo/ideapad-l340-15irh-gaming-81lk-2.bin`
- <https://github.com/5T33Z0/Lenovo-ThinkPad-E14-Gen-5-AMD-OpenCore>, commit `4cdc4e083de8`:
  - `data/lenovo/thinkpad-e14-gen-5-21jr.bin`
- <https://github.com/Baio1977/Lenovo-Thinkpad-T14>, commit `a5ce0b6a1522`:
  - `data/lenovo/thinkpad-t14-gen-1-20s1.bin`
- <https://github.com/5T33Z0/Thinkpad-T490-Hackintosh-OpenCore>, commit `1e7122511761`:
  - `data/lenovo/thinkpad-t490-20n3.bin`
- <https://github.com/Baio1977/Lenovo-ThinkPad-X1-Tablet-Gen3>, commit `32fa08bffce2`:
  - `data/lenovo/thinkpad-x1-tablet-gen-3-20kk.bin`
- <https://github.com/Baio1977/Lenovo-ThinkPad-X280>, commit `356fa9aa6cb3`:
  - `data/lenovo/thinkpad-x280-20ke.bin`
- <https://github.com/Baio1977/Lenovo-V130-15IKB-Kabylake>, commit `60a04740b6e8`:
  - `data/lenovo/v130-15ikb-81hn.bin`
- <https://github.com/kennyluvvuu/RusyaHackintosh>, commit `71388a5469d4`:
  - `data/misc/x99h.bin`
