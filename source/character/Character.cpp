#include "Character.h"

#include "helpers.h"

Character::Character()
	: vbo(0), tbo(0), ntri(66918), nvtx(11200), phase(0), strafe_amount(0), strafe_target(0), crouched_amount(0), crouched_target(0), responsive(0)
{
	this->trajectory = new Trajectory();
	this->ik = new IK();

	load("./network/character_vertices.bin",
		 "./network/character_triangles.bin",
		 "./network/character_parents.bin",
		 "./network/character_xforms.bin");
}

Character::~Character()
{
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (tbo != 0)
	{
		glDeleteBuffers(1, &tbo);
		tbo = 0;
	}

	delete trajectory;
	delete ik;
}

glm::vec3 Character::getPosition()
{
	return trajectory->get_center_position();
}

void Character::render(GLSLProgram *shader, mat4 view, mat4 proj)
{
	if (vao == 0)
		return;

	glBindVertexArray(vao);

	glUniformMatrix4fv(glGetUniformLocation(shader->getHandle(), "joints"), JOINT_NUM, GL_FALSE, (float *)joint_mesh_xform);

	shader->setUniform("Kd", 0.2f, 0.2f, 0.2f);
	shader->setUniform("Ks", 0.7f, 0.7f, 0.7f);
	shader->setUniform("Ka", 0.1f, 0.1f, 0.1f);
	shader->setUniform("Shininess", 380.0f);

	mat4 model = mat4(1.0f);

	mat4 mv = view * model;
	shader->setUniform("ModelViewMatrix", mv);

	shader->setUniform("ProjectionMatrix", proj);
	shader->setUniform("ViewMatrix", view);
	shader->setUniform("MVP", proj * mv);

	glDrawElements(GL_TRIANGLES, ntri, GL_UNSIGNED_INT, (void *)0);

	glBindVertexArray(0);
}

void Character::reset_position(glm::vec2 position, Terrain *ter, Areas *areas)
{
	this->ter = ter;
	this->areas = areas;

	ArrayXf Yp = trajectory->pfnn->getYmean();

	glm::vec3 root_position = glm::vec3(position.x, ter->sample(position), position.y);
	glm::mat3 root_rotation = glm::mat3();

	trajectory->reset(root_position, root_rotation);

	for (int i = 0; i < JOINT_NUM; i++)
	{
		int opos = 8 + (((Trajectory::LENGTH / 2) / 10) * 4) + (JOINT_NUM * 3 * 0);
		int ovel = 8 + (((Trajectory::LENGTH / 2) / 10) * 4) + (JOINT_NUM * 3 * 1);
		int orot = 8 + (((Trajectory::LENGTH / 2) / 10) * 4) + (JOINT_NUM * 3 * 2);

		glm::vec3 pos = (root_rotation * glm::vec3(Yp(opos + i * 3 + 0), Yp(opos + i * 3 + 1), Yp(opos + i * 3 + 2))) + root_position;
		glm::vec3 vel = (root_rotation * glm::vec3(Yp(ovel + i * 3 + 0), Yp(ovel + i * 3 + 1), Yp(ovel + i * 3 + 2)));
		glm::mat3 rot = (root_rotation * glm::toMat3(quat_exp(glm::vec3(Yp(orot + i * 3 + 0), Yp(orot + i * 3 + 1), Yp(orot + i * 3 + 2)))));

		joint_positions[i] = pos;
		joint_velocities[i] = vel;
		joint_rotations[i] = rot;
	}

	phase = 0.0;

	ik->reset(root_position);
}

void Character::load(const char *filename_v, const char *filename_t, const char *filename_p, const char *filename_r)
{

	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (tbo != 0)
	{
		glDeleteBuffers(1, &tbo);
		tbo = 0;
	}

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &tbo);

	FILE *f;

	f = fopen(filename_v, "rb");
	float *vbo_data = (float *)malloc(sizeof(float) * 15 * nvtx);
	fread(vbo_data, sizeof(float) * 15 * nvtx, 1, f);
	fclose(f);

	f = fopen(filename_t, "rb");
	uint32_t *tbo_data = (uint32_t *)malloc(sizeof(uint32_t) * ntri);
	fread(tbo_data, sizeof(uint32_t) * ntri, 1, f);
	fclose(f);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 15 * nvtx, vbo_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * ntri, tbo_data, GL_STATIC_DRAW);

	free(vbo_data);
	free(tbo_data);

	f = fopen(filename_p, "rb");
	float fparents[JOINT_NUM];
	fread(fparents, sizeof(float) * JOINT_NUM, 1, f);
	for (int i = 0; i < JOINT_NUM; i++)
	{
		joint_parents[i] = (int)fparents[i];
	}
	fclose(f);

	f = fopen(filename_r, "rb");
	fread(glm::value_ptr(joint_rest_xform[0]), sizeof(float) * JOINT_NUM * 4 * 4, 1, f);
	for (int i = 0; i < JOINT_NUM; i++)
	{
		joint_rest_xform[i] = glm::transpose(joint_rest_xform[i]);
	}
	fclose(f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void *)(sizeof(float) * 0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void *)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void *)(sizeof(float) * 7));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void *)(sizeof(float) * 11));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
}

void Character::update_move(glm::vec2 direction_velocity, glm::vec3 cam_direct, int vel, int strafe, bool is_crouched)
{

	glm::vec3 trajectory_target_direction_new = glm::normalize(glm::vec3(cam_direct.x, 0.0, cam_direct.z));
	glm::mat3 trajectory_target_rotation = glm::mat3(glm::rotate(atan2f(
																	 trajectory_target_direction_new.x,
																	 trajectory_target_direction_new.z),
																 glm::vec3(0, 1, 0)));

	float target_vel_speed = 2.5 + 2.5 * ((vel / 32768.0) + 1.0);

	glm::vec3 trajectory_target_velocity_new = target_vel_speed * (trajectory_target_rotation * glm::vec3(direction_velocity.x / 32768.0, 0, direction_velocity.y / 32768.0));
	trajectory->target_vel = glm::mix(trajectory->target_vel, trajectory_target_velocity_new, EXTRA_VELOCITY_SMOOTH);

	strafe_target = ((strafe / 32768.0) + 1.0) / 2.0;
	strafe_amount = glm::mix(strafe_amount, strafe_target, EXTRA_STRAFE_SMOOTH);

	glm::vec3 trajectory_target_velocity_dir = glm::length(trajectory->target_vel) < 1e-05 ? trajectory->target_dir : glm::normalize(trajectory->target_vel);
	trajectory_target_direction_new = mix_directions(trajectory_target_velocity_dir, trajectory_target_direction_new, strafe_amount);
	trajectory->target_dir = mix_directions(trajectory->target_dir, trajectory_target_direction_new, EXTRA_DIRECTION_SMOOTH);

	crouched_target = is_crouched ? 1.0 : 0.0;
	crouched_amount = glm::mix(crouched_amount, crouched_target, EXTRA_CROUCHED_SMOOTH);

	trajectory->update_gait(vel, crouched_amount, EXTRA_GAIT_SMOOTH);

	trajectory->predict(responsive, strafe_amount, areas);

	trajectory->input(ter, JOINT_NUM, &root_position, &root_rotation, joint_positions, joint_velocities);


	trajectory->pfnn->predict(phase);

	build_local_transform();

	set_ik();

	trajectory->post_update(&phase, areas);
}

void Character::build_local_transform()
{
	for (int i = 0; i < JOINT_NUM; i++)
	{
		int opos = 8 + (((Trajectory::LENGTH / 2) / 10) * 4) + (JOINT_NUM * 3 * 0);
		int ovel = 8 + (((Trajectory::LENGTH / 2) / 10) * 4) + (JOINT_NUM * 3 * 1);
		int orot = 8 + (((Trajectory::LENGTH / 2) / 10) * 4) + (JOINT_NUM * 3 * 2);

		glm::vec3 pos = (root_rotation * trajectory->getPosition(opos, i)) + root_position;
		glm::vec3 vel = (root_rotation * trajectory->getVelocity(ovel, i));
		glm::mat3 rot = (root_rotation * glm::toMat3(quat_exp(trajectory->getRotation(orot, i))));

		joint_positions[i] = glm::mix(joint_positions[i] + vel, pos, EXTRA_JOINT_SMOOTH);
		joint_velocities[i] = vel;
		joint_rotations[i] = rot;

		joint_global_anim_xform[i] = glm::transpose(glm::mat4(
			rot[0][0], rot[1][0], rot[2][0], pos[0],
			rot[0][1], rot[1][1], rot[2][1], pos[1],
			rot[0][2], rot[1][2], rot[2][2], pos[2],
			0, 0, 0, 1));

		if (i == 0)
		{
			joint_anim_xform[i] = joint_global_anim_xform[i];
		}
		else
		{
			joint_anim_xform[i] = glm::inverse(joint_global_anim_xform[joint_parents[i]]) * joint_global_anim_xform[i];
		}
	}

	forward_kinematics();
}

void Character::forward_kinematics()
{

	for (int i = 0; i < JOINT_NUM; i++)
	{
		joint_global_anim_xform[i] = joint_anim_xform[i];
		joint_global_rest_xform[i] = joint_rest_xform[i];
		int j = joint_parents[i];
		while (j != -1)
		{
			joint_global_anim_xform[i] = joint_anim_xform[j] * joint_global_anim_xform[i];
			joint_global_rest_xform[i] = joint_rest_xform[j] * joint_global_rest_xform[i];
			j = joint_parents[j];
		}
		joint_mesh_xform[i] = joint_global_anim_xform[i] * glm::inverse(joint_global_rest_xform[i]);
	}
}

void Character::set_ik()
{

	glm::vec4 ik_weight = glm::vec4(
		trajectory->pfnn->Yp(4 + 0),
		trajectory->pfnn->Yp(4 + 1),
		trajectory->pfnn->Yp(4 + 2),
		trajectory->pfnn->Yp(4 + 3));

	glm::vec3 key_hl = glm::vec3(joint_global_anim_xform[JOINT_HEEL_L][3]);
	glm::vec3 key_tl = glm::vec3(joint_global_anim_xform[JOINT_TOE_L][3]);
	glm::vec3 key_hr = glm::vec3(joint_global_anim_xform[JOINT_HEEL_R][3]);
	glm::vec3 key_tr = glm::vec3(joint_global_anim_xform[JOINT_TOE_R][3]);

	key_hl = glm::mix(key_hl, ik->position[IK::HL], ik->lock[IK::HL]);
	key_tl = glm::mix(key_tl, ik->position[IK::TL], ik->lock[IK::TL]);
	key_hr = glm::mix(key_hr, ik->position[IK::HR], ik->lock[IK::HR]);
	key_tr = glm::mix(key_tr, ik->position[IK::TR], ik->lock[IK::TR]);

	ik->height[IK::HL] = glm::mix(ik->height[IK::HL], ter->sample(glm::vec2(key_hl.x, key_hl.z)) + ik->heel_height, ik->smoothness);
	ik->height[IK::TL] = glm::mix(ik->height[IK::TL], ter->sample(glm::vec2(key_tl.x, key_tl.z)) + ik->toe_height, ik->smoothness);
	ik->height[IK::HR] = glm::mix(ik->height[IK::HR], ter->sample(glm::vec2(key_hr.x, key_hr.z)) + ik->heel_height, ik->smoothness);
	ik->height[IK::TR] = glm::mix(ik->height[IK::TR], ter->sample(glm::vec2(key_tr.x, key_tr.z)) + ik->toe_height, ik->smoothness);

	key_hl.y = glm::max(key_hl.y, ik->height[IK::HL]);
	key_tl.y = glm::max(key_tl.y, ik->height[IK::TL]);
	key_hr.y = glm::max(key_hr.y, ik->height[IK::HR]);
	key_tr.y = glm::max(key_tr.y, ik->height[IK::TR]);

	rotate_hip_knee(key_hl, key_hr);
	rotate_heel(ik_weight, key_tl, key_tr);
	rotate_toe(ik_weight);
	update_locks(ik_weight);
}

void Character::rotate_hip_knee(glm::vec3 key_hl, glm::vec3 key_hr)
{
	glm::vec3 hip_l = glm::vec3(joint_global_anim_xform[JOINT_HIP_L][3]);
	glm::vec3 knee_l = glm::vec3(joint_global_anim_xform[JOINT_KNEE_L][3]);
	glm::vec3 heel_l = glm::vec3(joint_global_anim_xform[JOINT_HEEL_L][3]);

	glm::vec3 hip_r = glm::vec3(joint_global_anim_xform[JOINT_HIP_R][3]);
	glm::vec3 knee_r = glm::vec3(joint_global_anim_xform[JOINT_KNEE_R][3]);
	glm::vec3 heel_r = glm::vec3(joint_global_anim_xform[JOINT_HEEL_R][3]);

	ik->two_joint(hip_l, knee_l, heel_l, key_hl, 1.0,
				  joint_global_anim_xform[JOINT_ROOT_L],
				  joint_global_anim_xform[JOINT_HIP_L],
				  joint_global_anim_xform[JOINT_HIP_L],
				  joint_global_anim_xform[JOINT_KNEE_L],
				  joint_anim_xform[JOINT_HIP_L],
				  joint_anim_xform[JOINT_KNEE_L]);

	ik->two_joint(hip_r, knee_r, heel_r, key_hr, 1.0,
				  joint_global_anim_xform[JOINT_ROOT_R],
				  joint_global_anim_xform[JOINT_HIP_R],
				  joint_global_anim_xform[JOINT_HIP_R],
				  joint_global_anim_xform[JOINT_KNEE_R],
				  joint_anim_xform[JOINT_HIP_R],
				  joint_anim_xform[JOINT_KNEE_R]);

	forward_kinematics();
}

void Character::rotate_heel(glm::vec4 ik_weight, glm::vec3 key_tl, glm::vec3 key_tr)
{
	const float heel_max_bend_s = 4;
	const float heel_max_bend_u = 4;
	const float heel_max_bend_d = 4;

	glm::vec4 ik_toe_pos_blend = glm::clamp(ik_weight * 2.5f, 0.0f, 1.0f);

	glm::vec3 heel_l = glm::vec3(joint_global_anim_xform[JOINT_HEEL_L][3]);
	glm::vec4 side_h0_l = joint_global_anim_xform[JOINT_HEEL_L] * glm::vec4(10, 0, 0, 1);
	glm::vec4 side_h1_l = joint_global_anim_xform[JOINT_HEEL_L] * glm::vec4(-10, 0, 0, 1);
	glm::vec3 side0_l = glm::vec3(side_h0_l) / side_h0_l.w;
	glm::vec3 side1_l = glm::vec3(side_h1_l) / side_h1_l.w;
	glm::vec3 floor_l = key_tl;

	side0_l.y = glm::clamp(ter->sample(glm::vec2(side0_l.x, side0_l.z)) + ik->toe_height, heel_l.y - heel_max_bend_s, heel_l.y + heel_max_bend_s);
	side1_l.y = glm::clamp(ter->sample(glm::vec2(side1_l.x, side1_l.z)) + ik->toe_height, heel_l.y - heel_max_bend_s, heel_l.y + heel_max_bend_s);
	floor_l.y = glm::clamp(floor_l.y, heel_l.y - heel_max_bend_d, heel_l.y + heel_max_bend_u);

	glm::vec3 targ_z_l = glm::normalize(floor_l - heel_l);
	glm::vec3 targ_x_l = glm::normalize(side0_l - side1_l);
	glm::vec3 targ_y_l = glm::normalize(glm::cross(targ_x_l, targ_z_l));
	targ_x_l = glm::cross(targ_z_l, targ_y_l);

	joint_anim_xform[JOINT_HEEL_L] = mix_transforms(
		joint_anim_xform[JOINT_HEEL_L],
		glm::inverse(joint_global_anim_xform[JOINT_KNEE_L]) * glm::mat4(
																  glm::vec4(targ_x_l, 0),
																  glm::vec4(-targ_y_l, 0),
																  glm::vec4(targ_z_l, 0),
																  glm::vec4(heel_l, 1)),
		ik_toe_pos_blend.y);

	glm::vec3 heel_r = glm::vec3(joint_global_anim_xform[JOINT_HEEL_R][3]);
	glm::vec4 side_h0_r = joint_global_anim_xform[JOINT_HEEL_R] * glm::vec4(10, 0, 0, 1);
	glm::vec4 side_h1_r = joint_global_anim_xform[JOINT_HEEL_R] * glm::vec4(-10, 0, 0, 1);
	glm::vec3 side0_r = glm::vec3(side_h0_r) / side_h0_r.w;
	glm::vec3 side1_r = glm::vec3(side_h1_r) / side_h1_r.w;
	glm::vec3 floor_r = key_tr;

	side0_r.y = glm::clamp(ter->sample(glm::vec2(side0_r.x, side0_r.z)) + ik->toe_height, heel_r.y - heel_max_bend_s, heel_r.y + heel_max_bend_s);
	side1_r.y = glm::clamp(ter->sample(glm::vec2(side1_r.x, side1_r.z)) + ik->toe_height, heel_r.y - heel_max_bend_s, heel_r.y + heel_max_bend_s);
	floor_r.y = glm::clamp(floor_r.y, heel_r.y - heel_max_bend_d, heel_r.y + heel_max_bend_u);

	glm::vec3 targ_z_r = glm::normalize(floor_r - heel_r);
	glm::vec3 targ_x_r = glm::normalize(side0_r - side1_r);
	glm::vec3 targ_y_r = glm::normalize(glm::cross(targ_z_r, targ_x_r));
	targ_x_r = glm::cross(targ_z_r, targ_y_r);

	joint_anim_xform[JOINT_HEEL_R] = mix_transforms(
		joint_anim_xform[JOINT_HEEL_R],
		glm::inverse(joint_global_anim_xform[JOINT_KNEE_R]) * glm::mat4(
																  glm::vec4(-targ_x_r, 0),
																  glm::vec4(targ_y_r, 0),
																  glm::vec4(targ_z_r, 0),
																  glm::vec4(heel_r, 1)),
		ik_toe_pos_blend.w);

	forward_kinematics();
}

void Character::rotate_toe(glm::vec4 ik_weight)
{
	const float toe_max_bend_d = 0;
	const float toe_max_bend_u = 10;

	glm::vec4 ik_toe_rot_blend = glm::clamp(ik_weight * 2.5f, 0.0f, 1.0f);

	glm::vec3 toe_l = glm::vec3(joint_global_anim_xform[JOINT_TOE_L][3]);
	glm::vec4 fwrd_h_l = joint_global_anim_xform[JOINT_TOE_L] * glm::vec4(0, 0, 10, 1);
	glm::vec4 side_h0_l = joint_global_anim_xform[JOINT_TOE_L] * glm::vec4(10, 0, 0, 1);
	glm::vec4 side_h1_l = joint_global_anim_xform[JOINT_TOE_L] * glm::vec4(-10, 0, 0, 1);
	glm::vec3 fwrd_l = glm::vec3(fwrd_h_l) / fwrd_h_l.w;
	glm::vec3 side0_l = glm::vec3(side_h0_l) / side_h0_l.w;
	glm::vec3 side1_l = glm::vec3(side_h1_l) / side_h1_l.w;

	fwrd_l.y = glm::clamp(ter->sample(glm::vec2(fwrd_l.x, fwrd_l.z)) + ik->toe_height, toe_l.y - toe_max_bend_d, toe_l.y + toe_max_bend_u);
	side0_l.y = glm::clamp(ter->sample(glm::vec2(side0_l.x, side0_l.z)) + ik->toe_height, toe_l.y - toe_max_bend_d, toe_l.y + toe_max_bend_u);
	side1_l.y = glm::clamp(ter->sample(glm::vec2(side0_l.x, side1_l.z)) + ik->toe_height, toe_l.y - toe_max_bend_d, toe_l.y + toe_max_bend_u);

	glm::vec3 side_l = glm::normalize(side0_l - side1_l);
	fwrd_l = glm::normalize(fwrd_l - toe_l);
	glm::vec3 upwr_l = glm::normalize(glm::cross(side_l, fwrd_l));
	side_l = glm::cross(fwrd_l, upwr_l);

	joint_anim_xform[JOINT_TOE_L] = mix_transforms(
		joint_anim_xform[JOINT_TOE_L],
		glm::inverse(joint_global_anim_xform[JOINT_HEEL_L]) * glm::mat4(
																  glm::vec4(side_l, 0),
																  glm::vec4(-upwr_l, 0),
																  glm::vec4(fwrd_l, 0),
																  glm::vec4(toe_l, 1)),
		ik_toe_rot_blend.y);

	glm::vec3 toe_r = glm::vec3(joint_global_anim_xform[JOINT_TOE_R][3]);
	glm::vec4 fwrd_h_r = joint_global_anim_xform[JOINT_TOE_R] * glm::vec4(0, 0, 10, 1);
	glm::vec4 side_h0_r = joint_global_anim_xform[JOINT_TOE_R] * glm::vec4(10, 0, 0, 1);
	glm::vec4 side_h1_r = joint_global_anim_xform[JOINT_TOE_R] * glm::vec4(-10, 0, 0, 1);
	glm::vec3 fwrd_r = glm::vec3(fwrd_h_r) / fwrd_h_r.w;
	glm::vec3 side0_r = glm::vec3(side_h0_r) / side_h0_r.w;
	glm::vec3 side1_r = glm::vec3(side_h1_r) / side_h1_r.w;

	fwrd_r.y = glm::clamp(ter->sample(glm::vec2(fwrd_r.x, fwrd_r.z)) + ik->toe_height, toe_r.y - toe_max_bend_d, toe_r.y + toe_max_bend_u);
	side0_r.y = glm::clamp(ter->sample(glm::vec2(side0_r.x, side0_r.z)) + ik->toe_height, toe_r.y - toe_max_bend_d, toe_r.y + toe_max_bend_u);
	side1_r.y = glm::clamp(ter->sample(glm::vec2(side1_r.x, side1_r.z)) + ik->toe_height, toe_r.y - toe_max_bend_d, toe_r.y + toe_max_bend_u);

	glm::vec3 side_r = glm::normalize(side0_r - side1_r);
	fwrd_r = glm::normalize(fwrd_r - toe_r);
	glm::vec3 upwr_r = glm::normalize(glm::cross(side_r, fwrd_r));
	side_r = glm::cross(fwrd_r, upwr_r);

	joint_anim_xform[JOINT_TOE_R] = mix_transforms(
		joint_anim_xform[JOINT_TOE_R],
		glm::inverse(joint_global_anim_xform[JOINT_HEEL_R]) * glm::mat4(
																  glm::vec4(side_r, 0),
																  glm::vec4(-upwr_r, 0),
																  glm::vec4(fwrd_r, 0),
																  glm::vec4(toe_r, 1)),
		ik_toe_rot_blend.w);

	forward_kinematics();
}

void Character::update_locks(glm::vec4 ik_weight)
{
	if ((ik->lock[IK::HL] == 0.0) && (ik_weight.y >= ik->threshold))
	{
		ik->lock[IK::HL] = 1.0;
		ik->position[IK::HL] = glm::vec3(joint_global_anim_xform[JOINT_HEEL_L][3]);
		ik->lock[IK::TL] = 1.0;
		ik->position[IK::TL] = glm::vec3(joint_global_anim_xform[JOINT_TOE_L][3]);
	}

	if ((ik->lock[IK::HR] == 0.0) && (ik_weight.w >= ik->threshold))
	{
		ik->lock[IK::HR] = 1.0;
		ik->position[IK::HR] = glm::vec3(joint_global_anim_xform[JOINT_HEEL_R][3]);
		ik->lock[IK::TR] = 1.0;
		ik->position[IK::TR] = glm::vec3(joint_global_anim_xform[JOINT_TOE_R][3]);
	}

	if ((ik->lock[IK::HL] > 0.0) && (ik_weight.y < ik->threshold))
	{
		ik->lock[IK::HL] = glm::clamp(ik->lock[IK::HL] - ik->fade, 0.0f, 1.0f);
		ik->lock[IK::TL] = glm::clamp(ik->lock[IK::TL] - ik->fade, 0.0f, 1.0f);
	}

	if ((ik->lock[IK::HR] > 0.0) && (ik_weight.w < ik->threshold))
	{
		ik->lock[IK::HR] = glm::clamp(ik->lock[IK::HR] - ik->fade, 0.0f, 1.0f);
		ik->lock[IK::TR] = glm::clamp(ik->lock[IK::TR] - ik->fade, 0.0f, 1.0f);
	}
}
