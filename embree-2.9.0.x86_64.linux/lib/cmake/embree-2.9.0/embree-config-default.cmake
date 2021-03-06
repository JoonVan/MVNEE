## ======================================================================== ##
## Copyright 2009-2015 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

SET(EMBREE_VERSION 2.9.0)
SET(EMBREE_VERSION_MAJOR 2)
SET(EMBREE_VERSION_MINOR 9)
SET(EMBREE_VERSION_PATCH 0)
SET(EMBREE_VERSION_NOTE "")

SET(EMBREE_ISA AVX512KNL)
SET(EMBREE_XEON_PHI_SUPPORT ON)
SET(EMBREE_BUILD_TYPE Release)
SET(EMBREE_RAY_PACKETS ON)
SET(EMBREE_ISPC_SUPPORT ON)
SET(EMBREE_STATIC_LIB OFF)
SET(EMBREE_TUTORIALS ON)

SET(EMBREE_RAY_MASK OFF)
SET(EMBREE_STAT_COUNTERS OFF)
SET(EMBREE_BACKFACE_CULLING OFF)
SET(EMBREE_INTERSECTION_FILTER ON)
SET(EMBREE_INTERSECTION_FILTER_RESTORE ON)
SET(EMBREE_BUFFER_STRIDE ON)
SET(EMBREE_EXPORT_ALL_SYMBOLS OFF)
SET(EMBREE_ENABLE_RAYSTREAM_LOGGER OFF) 
SET(EMBREE_IGNORE_INVALID_RAYS OFF)
SET(EMBREE_TASKING_SYSTEM TBB)
