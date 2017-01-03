# Making sure we use C++ 2011 standard.
AM_CFLAGS   = $(CFLAGS) $(OPENMP_CFLAGS)
AM_CXXFLAGS = $(CXXFLAGS) $(OPENMP_CFLAGS)
AM_LDFLAGS  =
libSqlite3_a_CFLAGS =
#AM_CFLAGS = $(OPENMP_CFLAGS)
# SUBDIRS = octree

NBODY_DIR         = $(srcdir)/NBody
OCTREE_DIR        = $(srcdir)/octree
TCAD_DIR          = $(srcdir)/TCAD
TCAD_MATHLIB_DIR  = $(TCAD_DIR)/mathlib
TCAD_MESH_DIR     = $(TCAD_DIR)/mesh
TCAD_SOLVER_DIR   = $(TCAD_DIR)/solver
INCLUDE_DIR       = $(srcdir)/include
PHYSICS_DIR       = $(srcdir)/physics
UTILS_DIR         = $(srcdir)/utils
SQLITE3_DIR       = $(srcdir)/sqlite3
BHTREE_DIR        = $(srcdir)/BHTree

SQLITE3_BUNDLED_CFLAGS = \
  -O3 \
  -DSQLITE_ENABLE_FTS3=1 \
  -DSQLITE_ENABLE_FTS3_TOKENIZER=1 \
  -DSQLITE_ENABLE_COLUMN_METADATA=1 \
  -DSQLITE_SECURE_DELETE \
  -DSQLITE_ENABLE_UNLOCK_NOTIFY=1 \
  -DSQLITE_ENABLE_DBSTAT_VTAB=1 \
  -DSQLITE_THREADSAFE=2 \
  -ldl

AM_CFLAGS += \
	-I$(NBODY_DIR) \
	-I$(TCAD_DIR) \
	-I$(TCAD_MATHLIB_DIR) \
	-I$(TCAD_MESH_DIR) \
	-I$(TCAD_SOLVER_DIR) \
	-I$(INCLUDE_DIR) \
	-I$(UTILS_DIR) \
	-I$(OCTREE_DIR) \
	-I$(PHYSICS_DIR) \
	-I$(BHTREE_DIR)

AM_CXXFLAGS += \
	-I$(NBODY_DIR) \
	-I$(TCAD_DIR) \
	-I$(TCAD_MATHLIB_DIR) \
	-I$(TCAD_MESH_DIR) \
	-I$(TCAD_SOLVER_DIR) \
	-I$(INCLUDE_DIR) \
	-I$(UTILS_DIR) \
	-I$(OCTREE_DIR) \
	-I$(PHYSICS_DIR) \
	-I$(BHTREE_DIR)

if USE_BUNDLED_SQLITE3
  AM_CFLAGS += -I$(SQLITE3_DIR)
  AM_CXXFLAGS += -I$(SQLITE3_DIR)
endif

AM_LDFLAGS += \
	$(BOOST_LDFLAGS) \
	-L$(NBODY_DIR) \
	-L$(TCAD_DIR) \
	-L$(TCAD_MATHLIB_DIR) \
	-L$(TCAD_MESH_DIR) \
	-L$(TCAD_SOLVER_DIR) \
	-L$(INCLUDE_DIR) \
	-L$(UTILS_DIR) \
	-L$(PHYSICS_DIR) \
  -L$(BHTREE_DIR) \
	-ldl

if !USE_BUNDLED_SQLITE3
  lib_LIBRARIES = \
	libNBody.a \
	libpdTCAD.a \
	libPhysics.a
else
  lib_LIBRARIES = \
	libNBody.a \
	libpdTCAD.a \
	libPhysics.a \
	libSqlite3.a
endif

libNBody_a_SOURCES = \
	$(NBODY_DIR)/nbody.cc \
	$(NBODY_DIR)/nbody.h \
	$(NBODY_DIR)/nbody_octree.cc \
	$(NBODY_DIR)/nbody_octree.h \
	$(NBODY_DIR)/carrier.cc \
	$(NBODY_DIR)/carrier.h \
	$(NBODY_DIR)/visual.cc \
	$(NBODY_DIR)/visual.h \
	$(UTILS_DIR)/Utils.h \
	$(UTILS_DIR)/fputils.cc \
	$(UTILS_DIR)/fputils.h \
	$(UTILS_DIR)/readcsv.cc \
	$(UTILS_DIR)/readcsv.h \
	$(UTILS_DIR)/untar.cc \
	$(UTILS_DIR)/untar.h \
	$(UTILS_DIR)/pbar.cc \
	$(UTILS_DIR)/pbar.h \
	$(UTILS_DIR)/decor_output.cc \
	$(UTILS_DIR)/decor_output.h \
	$(NBODY_DIR)/load_carr.cc \
	$(NBODY_DIR)/load_carr.h \
	$(NBODY_DIR)/sim_file_io.cc \
	$(NBODY_DIR)/sim_file_io.h \
	$(NBODY_DIR)/sim_progress.cc \
	$(NBODY_DIR)/sim_progress.h \
	$(BHTREE_DIR)/Octant.h \
	$(BHTREE_DIR)/Octant.cc \
	$(BHTREE_DIR)/BHTree.cc \
	$(BHTREE_DIR)/BHTree.h

#libNBody_a_SOURCES = \
#	$(NBODY_DIR)/nbody.cc \
#	$(NBODY_DIR)/nbody.h \
#	$(NBODY_DIR)/nbody_octree.cc \
#	$(NBODY_DIR)/nbody_octree.h \
#	$(NBODY_DIR)/carrier.cc \
#	$(NBODY_DIR)/carrier.h \
#	$(NBODY_DIR)/visual.cc \
#	$(NBODY_DIR)/visual.h \
#	$(UTILS_DIR)/Utils.h \
#	$(UTILS_DIR)/fputils.cc \
#	$(UTILS_DIR)/fputils.h \
#	$(UTILS_DIR)/readcsv.cc \
#	$(UTILS_DIR)/readcsv.h \
#	$(UTILS_DIR)/untar.cc \
#	$(UTILS_DIR)/untar.h \
#	$(UTILS_DIR)/pbar.cc \
#	$(UTILS_DIR)/pbar.h \
#	$(UTILS_DIR)/decor_output.cc \
#	$(UTILS_DIR)/decor_output.h \
#	$(NBODY_DIR)/load_carr.cc \
#	$(NBODY_DIR)/load_carr.h \
#	$(NBODY_DIR)/sim_file_io.cc \
#	$(NBODY_DIR)/sim_file_io.h \
#	$(NBODY_DIR)/sim_progress.cc \
#	$(NBODY_DIR)/sim_progress.h \
#	$(OCTREE_DIR)/octree.tcc \
#	$(OCTREE_DIR)/octree.h \
#	$(OCTREE_DIR)/tvector.tcc \
#	$(OCTREE_DIR)/tvector.h \
#	$(OCTREE_DIR)/tarray.tcc \
#	$(OCTREE_DIR)/tarray.h \
#	$(OCTREE_DIR)/tarray2D.h \
#	$(OCTREE_DIR)/tcoord3D.h \
#	$(OCTREE_DIR)/prefer_fp.h \
#	$(BHTREE_DIR)/Octant.h \
#	$(BHTREE_DIR)/Octant.cc \
#	$(BHTREE_DIR)/BHTree.cc \
#	$(BHTREE_DIR)/BHTree.h

libpdTCAD_a_SOURCES = \
	$(TCAD_DIR)/pd_tcad.cc \
	$(TCAD_DIR)/pd_tcad.h \
	$(TCAD_MESH_DIR)/pd_tcad_mesh.cc \
	$(TCAD_MESH_DIR)/pd_tcad_mesh.h \
	$(TCAD_SOLVER_DIR)/pd_tcad_solver.cc \
	$(TCAD_SOLVER_DIR)/pd_tcad_solver.h \
	$(TCAD_MESH_DIR)/pd_tcad_node.cc \
	$(TCAD_MESH_DIR)/pd_tcad_node.h \
	$(TCAD_MATHLIB_DIR)/t_matrix.tcc \
	$(TCAD_MATHLIB_DIR)/t_matrix.h \
	$(UTILS_DIR)/datatype_hash.cc \
	$(UTILS_DIR)/datatype_hash.h

libPhysics_a_SOURCES = \
	$(PHYSICS_DIR)/physical_constants.cc \
	$(PHYSICS_DIR)/physical_constants.h \
	$(PHYSICS_DIR)/materials.cc \
	$(PHYSICS_DIR)/materials.h \
	$(PHYSICS_DIR)/recombination.cc \
	$(PHYSICS_DIR)/recombination.h \
	$(PHYSICS_DIR)/CTCForce.cc \
	$(PHYSICS_DIR)/CTCForce.h \
	$(PHYSICS_DIR)/sim_space.cc \
	$(PHYSICS_DIR)/sim_space.h \
	$(PHYSICS_DIR)/recombination_nbody.h \
	$(PHYSICS_DIR)/recombination_nbody.cc \
	$(PHYSICS_DIR)/SimCondition.h \
	$(PHYSICS_DIR)/SimCondition.cc

if USE_BUNDLED_SQLITE3
  libSqlite3_a_SOURCES = \
	$(SQLITE3_DIR)/sqlite3.h \
	$(SQLITE3_DIR)/sqlite3.c \
	$(SQLITE3_DIR)/sqlite3ext.h
endif

# The main program
bin_PROGRAMS = pdelay
pdelay_SOURCES = \
	$(srcdir)/pdelay.cc \
	$(srcdir)/pdelay.h \
	$(srcdir)/sim_dimension.h

if !USE_BUNDLED_SQLITE3
	pdelay_LDADD = \
	$(srcdir)/libNBody.a \
	$(srcdir)/libpdTCAD.a \
	$(srcdir)/libPhysics.a \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_IOSTREAMS_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(SQLITE3_LDFLAGS)
else
  pdelay_LDADD = \
	$(srcdir)/libNBody.a \
	$(srcdir)/libpdTCAD.a \
	$(srcdir)/libPhysics.a \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_IOSTREAMS_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(srcdir)/libSqlite3.a
endif

# SQLite3
if !USE_BUNDLED_SQLITE3
	AM_CXXFLAGS += $(SQLITE3_CFLAGS)
else
	libSqlite3_a_CFLAGS += $(SQLITE3_BUNDLED_CFLAGS)
endif
