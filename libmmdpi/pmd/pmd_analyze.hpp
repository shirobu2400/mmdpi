
#include "pmd_load.hpp"
#include "../model/mmdpi_model.hpp"

#ifndef		__MMDPI__PMD__ANALYZE__DEFINES__
#define		__MMDPI__PMD__ANALYZE__DEFINES__	( 1 )

class mmdpiPmdAnalyze : public mmdpiPmdLoad, public mmdpiModel
{
protected:

	MMDPI_MATERIAL_PTR		adjust_material;
	MMDPI_BLOCK_VERTEX_PTR		adjust_vertex;

	MMDPI_IMAGE*			texture;
	MMDPI_IMAGE*			toon_texture;

	void				load_texture( void );
	int				analyze( void );
	int				create_bone( MMDPI_PMD_BONE_INFO_PTR pbone, uint pbone_len );

public:

	virtual	int			load( const char *file_name );

	mmdpiPmdAnalyze();
	~mmdpiPmdAnalyze();
};

#endif	//	__MMDPI__PMD__ANALYZE__DEFINES__
