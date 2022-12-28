#include "mmdpi_include.hpp"
#include "mmdpi_struct.hpp"
#include "mmdpi_shader.hpp"

#ifndef		__MMDPI__MESH__DEFINES__
#define		__MMDPI__MESH__DEFINES__	( 1 )

class mmdpiMesh
{
public:
	int				id;

	dword				b_vertex_num;
	MMDPI_VERTEX*			b_vertex;

	dword				b_face_num;
	mmdpiShaderIndex*		b_face;

	MMDPI_PIECE*			b_piece;

	mmdpiShader			( *shader );

	int				draw( void );

	int				draw_main( int cull_flag );

	void				update_gpu_cache_matrix( MMDPI_BONE_INFO_PTR bone, dword bone_num );

	int				set_vertex( const MMDPI_VERTEX* vertex, dword vertex_num );
	int				set_face( const mmdpiShaderIndex* face, dword face_num );
	int				set_material( dword raw_material_id, MMDPI_MATERIAL* raw_material );
	int				set_boneid( const dword* boneid, dword boneid_length );

	mmdpiMesh( mmdpiShader* shader_p );
	~mmdpiMesh();
};
#endif	//	__MMDPI__MESH__DEFINES__
