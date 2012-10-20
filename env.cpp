/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#include "env.h"
#include "ogl.h"
#include "textures.h"
#include "particles.h"
#include "spx.h"
#include "view.h"
#include "course.h"

// --------------------------------------------------------------------
//					defaults
// --------------------------------------------------------------------

static float def_amb[]     = {0.45, 0.53, 0.75, 1.0};    
static float def_diff[]    = {1.0, 0.9, 1.0, 1.0};    
static float def_spec[]    = {0.6, 0.6, 0.6, 1.0};    
static float def_pos[]     = {1, 2, 2, 0.0};    
static float def_fogcol[]  = {0.9, 0.9, 1.0, 0.0};
static float def_partcol[] = {0.8, 0.8, 0.9, 0.0};

CEnvironment Env;

CEnvironment::CEnvironment () {
	int i;
	EnvID = -1;
	for (i=0; i<MAX_LOCATIONS; i++) locs[i].name = "";
	numLocs = 0;
	EnvDir = "";
	LightIndex = "[sunny]0[cloudy]1[evening]2[night]3";
	lightcond[0].name = "sunny";
	lightcond[1].name = "cloudy";
	lightcond[2].name = "evening";
	lightcond[3].name = "night";
	for (i=0; i<6; i++) Skybox[i] = 0;

	default_light.is_on = true;
	for (int i=0; i<4; i++) { 
		default_light.ambient[i]  = def_amb[i];
		default_light.diffuse[i]  = def_diff[i];
		default_light.specular[i] = def_spec[i];
		default_light.position[i] = def_pos[i];
	}

	default_fog.is_on = true;
	default_fog.mode = GL_LINEAR;
	default_fog.start = 20.0;
	default_fog.end = 70.0;
	default_fog.height = 0;
	for (int i=0; i<4; i++) { 
		default_fog.color[i] =  def_fogcol[i];
		default_fog.part_color[i] =  def_partcol[i];
	}
}

CEnvironment::~CEnvironment () {}

void CEnvironment::ResetSkybox () {
	for (int i=0; i<6; i++) {
		if (Skybox[i] != 0) glDeleteTextures (1, &Skybox[i]);
		Skybox[i] = 0;
	}
}

void CEnvironment::SetupLight () {
	glLightfv (GL_LIGHT0, GL_POSITION, lights[0].position);
	glLightfv (GL_LIGHT0, GL_AMBIENT, lights[0].ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, lights[0].diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, lights[0].specular);
	glEnable  (GL_LIGHT0);
	if (lights[1].is_on) {
		glLightfv (GL_LIGHT1, GL_POSITION, lights[1].position);
		glLightfv (GL_LIGHT1, GL_AMBIENT, lights[1].ambient);
		glLightfv (GL_LIGHT1, GL_DIFFUSE, lights[1].diffuse);
		glLightfv (GL_LIGHT1, GL_SPECULAR, lights[1].specular);
		glEnable  (GL_LIGHT1);
	}
	if (lights[2].is_on) {
		glLightfv (GL_LIGHT2, GL_POSITION, lights[2].position);
		glLightfv (GL_LIGHT2, GL_AMBIENT, lights[2].ambient);
		glLightfv (GL_LIGHT2, GL_DIFFUSE, lights[2].diffuse);
		glLightfv (GL_LIGHT2, GL_SPECULAR, lights[2].specular);
		glEnable  (GL_LIGHT2);
	}
	if (lights[3].is_on) {
		glLightfv (GL_LIGHT3, GL_POSITION, lights[3].position);
		glLightfv (GL_LIGHT3, GL_AMBIENT, lights[3].ambient);
		glLightfv (GL_LIGHT3, GL_DIFFUSE, lights[3].diffuse);
		glLightfv (GL_LIGHT3, GL_SPECULAR, lights[3].specular);
		glEnable  (GL_LIGHT3);
	}
	glEnable  (GL_LIGHTING);
}

void CEnvironment::SetupFog () {
    glEnable (GL_FOG);
    glFogf   (GL_FOG_MODE, fog.mode);
    glFogf   (GL_FOG_START, fog.start);
    glFogf   (GL_FOG_END, fog.end);
    glFogfv  (GL_FOG_COLOR, fog.color);

    if (param.perf_level > 1) {
		glHint (GL_FOG_HINT, GL_NICEST );
    } else {
		glHint (GL_FOG_HINT, GL_FASTEST );
    }
}

void CEnvironment::ResetLight () {
	lights[0] = default_light;
	for (int i=1; i<4; i++) lights[i].is_on = false;
	glDisable (GL_LIGHT1);
	glDisable (GL_LIGHT2);
	glDisable (GL_LIGHT3);
}

void CEnvironment::ResetFog () { 
	fog = default_fog; 
}

void CEnvironment::Reset () {
	ResetSkybox ();
	ResetLight ();
	ResetFog ();
}

bool CEnvironment::LoadEnvironmentList () {
	int i;
	string line;

	CSPList list (32, true);
	if (!list.Load (param.env_dir2, "environment.lst")) {
		Message ("could not load environment.lst");
		return false;
	}

	numLocs = 0;
	for (i=0; i<list.Count(); i++) {
		line = list.Line (i);
		locs[i].name = SPStrN (line, "location", "");
		numLocs++;
	}
	list.MakeIndex (EnvIndex, "location");
	return true;
}

string CEnvironment::GetDir (int location, int light) {
	if (location < 0 || location >= numLocs) return "";
	if (light < 0 || light >= 4) return "";
	string res = 
		param.env_dir2 + SEP + 
		locs[location].name + SEP + lightcond[light].name;
	return res;
}

void CEnvironment::LoadSkybox () {
	Skybox[0] = Tex.LoadTexture (EnvDir.c_str(), "front.png");
	Skybox[1] = Tex.LoadTexture (EnvDir.c_str(), "left.png");
	Skybox[2] = Tex.LoadTexture (EnvDir.c_str(), "right.png");
	if (param.full_skybox) {
		Skybox[3] = Tex.LoadTexture (EnvDir.c_str(), "top.png");
		Skybox[4] = Tex.LoadTexture (EnvDir.c_str(), "bottom.png");
		Skybox[5] = Tex.LoadTexture (EnvDir.c_str(), "back.png");
	}
}

void CEnvironment::LoadLight () {
	string idxstr = "[fog]-1[0]0[1]1[2]2[3]3[4]4[5]5[6]6";
	string line;
	string item;
	int i, idx;	

	CSPList list(24);
	if (!list.Load (EnvDir.c_str(), "light.lst")) {
		Message ("could not load light file", "");
		return;
	}

	for (i=0; i<list.Count(); i++) {
		line = list.Line(i);
		item = SPStrN (line, "light", "none");
		idx = SPIntN (idxstr, item, -1);
		if (idx < 0) {
			fog.is_on = SPBoolN (line, "fog", true);
			fog.start = SPFloatN (line, "fogstart", 20);
			fog.end = SPFloatN (line, "fogend", param.forward_clip_distance);
			fog.height = SPFloatN (line, "fogheight", 0);
			SPArrN (line, "fogcol", fog.color, 4, 1);
			SPArrN (line, "partcol", fog.part_color, 4, 1);
		} else if (idx < 4){
			lights[idx].is_on = true;
			SPArrN (line, "amb", lights[idx].ambient, 4, 1);
			SPArrN (line, "diff", lights[idx].diffuse, 4, 1);
			SPArrN (line, "spec", lights[idx].specular, 4, 1);
			SPArrN (line, "pos", lights[idx].position, 4, 1);
		}
	}
}

void CEnvironment::DrawSkybox (TVector3 pos) {
 	set_gl_options (SKY);
	double aa, bb;
	
#if defined (OS_LINUX)
	aa = 0.0;
	bb = 1.0;
#else
	aa = 0.005;
	bb = 0.995;
#endif

	glColor4f (1.0, 1.0, 1.0, 1.0);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	glTranslatef (pos.x, pos.y, pos.z);
	
	const GLfloat tex[] = {
		aa, aa,
		bb, aa,
		bb, bb,
		aa, bb
	};

	// front
	const GLfloat front[] = {
		-1, -1, -1,
		 1, -1, -1,
		 1,  1, -1,
		-1,  1, -1		
	};

	glBindTexture (GL_TEXTURE_2D, Skybox[0]);
	glVertexPointer(3, GL_FLOAT, 0, front);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	
	// left
	const GLfloat left[] = {
		-1, -1,  1,
		-1, -1, -1,
		-1,  1, -1,
		-1,  1,  1		
	};

	glBindTexture (GL_TEXTURE_2D, Skybox[1]);
	glVertexPointer(3, GL_FLOAT, 0, left);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	// right
	const GLfloat right[] = {
		 1, -1, -1,
		 1, -1,  1,
		 1,  1,  1,
		 1,  1, -1		
	};

	glBindTexture (GL_TEXTURE_2D, Skybox[2]);
	glVertexPointer(3, GL_FLOAT, 0, right);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	// normally, the following textures are unvisible
	// see game_config.cpp (param.full_skybox)
	if (param.full_skybox) {
		// top
		const GLfloat top[] = {
			-1,  1, -1,
			 1,  1, -1,
			 1,  1,  1,
			-1,  1,  1
		};

		glBindTexture (GL_TEXTURE_2D, Skybox[3]);
		glVertexPointer(3, GL_FLOAT, 0, top);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		
		// bottom
		const GLfloat bottom[] = {
			-1, -1,  1,
			 1, -1,  1,
			 1, -1, -1,
			-1, -1, -1
		};

		glBindTexture (GL_TEXTURE_2D, Skybox[4]);
		glVertexPointer(3, GL_FLOAT, 0, bottom);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		
		// back
		const GLfloat back[] = {
			 1, -1,  1,
			-1, -1,  1,
			-1,  1,  1,
			 1,  1,  1
		};

		glBindTexture (GL_TEXTURE_2D, Skybox[5]);
		glVertexPointer(3, GL_FLOAT, 0, back);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
	}	
	glPopMatrix();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CEnvironment::DrawFog () {
    TPlane bottom_plane, top_plane;
    TVector3 left, right;
    TVector3 topleft, topright;
    TVector3 bottomleft = NullVec; 
    TVector3 bottomright = NullVec;
    float height;

    if (!fog.is_on) return;

    // the clipping planes are calculated by view frustum (view.cpp)
    leftclip = get_left_clip_plane ();
    rightclip = get_right_clip_plane ();
    farclip = get_far_clip_plane ();
    bottomclip = get_bottom_clip_plane ();

    // --------------- calculate the planes ---------------------------

    float slope = tan (ANGLES_TO_RADIANS (Course.GetCourseAngle()));
//	TPlane left_edge_plane = MakePlane (1.0, 0.0, 0.0, 0.0);
//	TPlane right_edge_plane = MakePlane (-1.0, 0.0, 0.0, Course.width);

    bottom_plane.nml = MakeVector (0.0, 1, -slope);
    height = Course.GetBaseHeight (0);
    bottom_plane.d = -height * bottom_plane.nml.y;

    top_plane.nml = bottom_plane.nml;
    height = Course.GetMaxHeight (0) + fog.height;
    top_plane.d = -height * top_plane.nml.y;

    if (!IntersectPlanes (bottom_plane, farclip, leftclip,  &left)) return;
    if (!IntersectPlanes (bottom_plane, farclip, rightclip, &right)) return;
    if (!IntersectPlanes (top_plane,    farclip, leftclip,  &topleft)) return;
    if (!IntersectPlanes (top_plane,    farclip, rightclip, &topright)) return;
    if (!IntersectPlanes (bottomclip,   farclip, leftclip,  &bottomleft)) return;
    if (!IntersectPlanes (bottomclip,   farclip, rightclip, &bottomright)) return;

    TVector3 leftvec  = SubtractVectors (topleft, left);
    TVector3 rightvec = SubtractVectors (topright, right);

    TVector3 vpoint1 = AddVectors (topleft, leftvec);
    TVector3 vpoint2 = AddVectors (topright, rightvec);
    TVector3 vpoint3 = AddVectors (topleft, ScaleVector (3.0, leftvec));
    TVector3 vpoint4 = AddVectors (topright, ScaleVector (3.0, rightvec));

    // --------------- draw the fog plane -----------------------------

    set_gl_options (FOG_PLANE);
    glEnable (GL_FOG);

    // only the alpha channel is used
    float bottom_dens[4]     = {0, 0, 0, 1.0};
    float top_dens[4]        = {0, 0, 0, 0.9};
    float leftright_dens[4]  = {0, 0, 0, 0.3};
    float top_bottom_dens[4] = {0, 0, 0, 0.0};

#if 0
    //glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

	// only the alpha channel is used
	const GLfloat col[] = {
		// bottom density
		0, 0, 0, 1.0,
		0, 0, 0, 1.0,
		0, 0, 0, 1.0,
		0, 0, 0, 1.0,

		// top density
		0, 0, 0, 0.9,
		0, 0, 0, 0.9,

		// left/right density
		0, 0, 0, 0.3,
		0, 0, 0, 0.3,

		// top/bottom density
		0, 0, 0, 0.0,
		0, 0, 0, 0.0,
	};

	const GLfloat vtx[] = {
		bottomleft.x, bottomleft.y, bottomleft.z,
		bottomright.x, bottomright.y, bottomright.z,
		left.x, left.y, left.z,
		right.x, right.y, right.z,
		topleft.x, topleft.y, topleft.z,
		topright.x, topright.y, topright.z,
		vpoint1.x, vpoint1.y, vpoint1.z,
		vpoint2.x, vpoint2.y, vpoint2.z,
		vpoint3.x, vpoint3.y, vpoint3.z,
		vpoint4.x, vpoint4.y, vpoint4.z,
	};

	glColor4f (bottom_dens[0], bottom_dens[1], bottom_dens[2], bottom_dens[3]);
	//glColorPointer(4, GL_FLOAT, 0, col+0);
	glVertexPointer(3, GL_FLOAT, 0, vtx+0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glColor4f (top_dens[0], top_dens[1], top_dens[2], top_dens[3]);
	//glColorPointer(4, GL_FLOAT, 0, col+8);
	glVertexPointer(3, GL_FLOAT, 0, vtx+6);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glColor4f (leftright_dens[0], leftright_dens[1], leftright_dens[2], leftright_dens[3]);
	//glColorPointer(4, GL_FLOAT, 0, col+16);
	glVertexPointer(3, GL_FLOAT, 0, vtx+12);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glColor4f (top_bottom_dens[0], top_bottom_dens[1], top_bottom_dens[2], top_bottom_dens[3]);
	//glColorPointer(4, GL_FLOAT, 0, col+24);
	glVertexPointer(3, GL_FLOAT, 0, vtx+18);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);

#else
    glBegin (GL_QUAD_STRIP);
	glColor4f (bottom_dens[0], bottom_dens[1], bottom_dens[2], bottom_dens[3]);
	glVertex3f (bottomleft.x, bottomleft.y, bottomleft.z);
    	glVertex3f (bottomright.x, bottomright.y, bottomright.z);
	glVertex3f (left.x, left.y, left.z);
    	glVertex3f (right.x, right.y, right.z);

	glColor4f (top_dens[0], top_dens[1], top_dens[2], top_dens[3]);
    	glVertex3f (topleft.x, topleft.y, topleft.z);
    	glVertex3f (topright.x, topright.y, topright.z);
	
	glColor4f (leftright_dens[0], leftright_dens[1], leftright_dens[2], leftright_dens[3]);
   	glVertex3f (vpoint1.x, vpoint1.y, vpoint1.z);
	glVertex3f (vpoint2.x, vpoint2.y, vpoint2.z);
		
	glColor4f (top_bottom_dens[0], top_bottom_dens[1], top_bottom_dens[2], top_bottom_dens[3]);
    	glVertex3f (vpoint3.x, vpoint3.y, vpoint3.z);
    	glVertex3f (vpoint4.x, vpoint4.y, vpoint4.z);
    glEnd();

#endif
}


bool CEnvironment::LoadEnvironment (int loc, int light) {
	if (loc < 0 || loc >= numLocs) loc = 0;
	if (light < 0 || light >= 4) light = 0;
	// remember: with (example) 3 locations and 4 lights there
	// are 12 different environments
	int env_id = loc * 100 + light;

	if (env_id == EnvID) {
		Message ("same environment");
		return true;
	}

	// Set directory. The dir is used several times.
	EnvDir = GetDir (loc, light);

	// Load skybox. If the sky can't be loaded for any reason, the 
	// texture id's are set to 0 and the sky will not be drawn.
	// There is no error handlung, you see the result on the screen.
	ResetSkybox ();
	LoadSkybox ();

	// Load light conditions. 
	ResetFog ();
	ResetLight ();
	LoadLight ();
	return true;
}

TColor CEnvironment::ParticleColor () {
	TColor res;
	res.r = fog.part_color[0];
	res.g = fog.part_color[1];
	res.b = fog.part_color[2];
	res.a = fog.part_color[3];
	return res;
}

int CEnvironment::GetEnvIdx (const string tag) {
	return SPIntN (EnvIndex, tag, 0);
}

int CEnvironment::GetLightIdx (const string tag) {
	return SPIntN (LightIndex, tag, 0);
}








