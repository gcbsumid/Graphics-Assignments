-- Frieza

rootnode = gr.node('root')

purple = gr.material( {0.30, 0.0, 0.955}, {0.2, 0.2, 0.2}, 3);
grey = gr.material( {0.8, 0.8, 0.8}, { 0.1, 0.1, 0.1}, 10)
dirty_white = gr.material( {0.2, .2, .2}, { 0.2, 0.2, 0.2}, 10)
red = gr.material( {1.0, 0.2, 0.2}, { 0.2, 0.2, 0.2}, 10)

-- Start of the body
torso = gr.sphere('torso')
rootnode:add_child(torso)
torso:scale(2.0, 3.0, 1.3)
torso:set_material(grey);

-- Chest
left_pectorial = gr.sphere('left_pectorial')
torso:add_child(left_pectorial)
left_pectorial:scale(1.6, 1.3, 1)
left_pectorial:translate(1.0, 1.5, -1.0);
left_pectorial:set_material(grey)

right_pectorial = gr.sphere('right_pectorial')
torso:add_child(right_pectorial)
right_pectorial:scale(1.6, 1.3, 1)
right_pectorial:translate(-1.0, 1.5, -1.0);
right_pectorial:set_material(grey)

plexus_top = gr.sphere('plexus_top')
torso:add_child(plexus_top)
plexus_top:scale(0.85, 0.75, 0.5);
plexus_top:translate(0.0, 0.85, -1.5);
plexus_top:set_material(purple)

plexus_bottom = gr.sphere('plexus_bottom')
torso:add_child(plexus_bottom)
plexus_bottom:scale(0.55, 0.75, 0.35);
plexus_bottom:translate(0.0, 0.55, -1.5);
plexus_bottom:rotate('x', -20);
plexus_bottom:set_material(purple)

-- Shoulders
shoulders = gr.sphere('shoulders')
torso:add_child(shoulders)
shoulders:scale(3.0, 1.5, 1.75);
shoulders:translate(0.0, 1.5, 0.0);
shoulders:set_material(grey)

-- neck
neck_joint = gr.joint('neck_joint', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
shoulders:add_child(neck_joint)

neck = gr.sphere('neck')
neck_joint:add_child(neck)
neck:scale(0.65, 1.0, 0.65)
neck:translate(0.0, 1.7, 0.0);
neck:set_material(grey);

left_neck = gr.sphere('left_neck')
neck:add_child(left_neck)
left_neck:scale(0.45, 0.65, 0.55)
left_neck:translate(-.5, 0, 0);
left_neck:rotate('z', -30)
left_neck:set_material(grey);

right_neck = gr.sphere('right_neck')
neck:add_child(right_neck)
right_neck:scale(0.45, 0.65, 0.55)
right_neck:translate(0.5, 0, 0.0);
right_neck:rotate('z', 30)
right_neck:set_material(grey);

head_joint = gr.joint('head_joint_x', {-45.0, 0.0, 45.0}, {-65.0, 0.0, 65.0})
neck:add_child(head_joint);

head = gr.sphere('head')
head_joint:add_child(head)
head:scale(1.3, 1.5, 1.5)
head:translate(0.0, 1.4, 0.0);
head:set_material(grey);

head_top = gr.sphere('head_top')
head:add_child(head_top)
head_top:scale(1.0, 1.2, 1.6)
head_top:translate(0.0, 0.4, 0)
head_top:set_material(purple)

head_ears = gr.sphere('head_ears')
head:add_child(head_ears)
head_ears:scale(1.5, 1, 0.8)
head_ears:set_material(grey)

left_eye = gr.sphere('left_eye')
head:add_child(left_eye)
left_eye:scale(0.35, 0.2, 0.25)
left_eye:translate(-0.45, -0.1, -1.25);
left_eye:rotate('z', -25)
left_eye:rotate('y', 35)
left_eye:set_material(grey);

left_pupil = gr.sphere('left_pupil')
left_eye:add_child(left_pupil)
left_pupil:scale(0.1, 0.1, 0.1)
left_pupil:translate(0.2, -0.025, -0.1);
left_pupil:set_material(red);

right_eye = gr.sphere('right_eye')
head:add_child(right_eye)
right_eye:scale(0.35, 0.2, 0.25)
right_eye:translate(0.45, -0.1, -1.25);
right_eye:rotate('z', 25)
right_eye:rotate('y', -35)
right_eye:set_material(grey);

right_pupil = gr.sphere('right_pupil')
right_eye:add_child(right_pupil)
right_pupil:scale(0.1, 0.1, 0.1)
right_pupil:translate(-0.2, -0.025, -0.1);
right_pupil:set_material(red);

nose = gr.sphere('nose')
head:add_child(nose)
nose:scale(0.08, 0.1, 0.08)
nose:translate(0.0, -.7, -1.35);
nose:set_material(grey);

mouth = gr.sphere('mouth')
nose:add_child(mouth)
mouth:scale(0.12, 0.08, 0.08)
mouth:translate(0.0, -.4, .2);
mouth:set_material(purple);

-- hips 
hips_top = gr.sphere('hips_top')
torso:add_child(hips_top)
hips_top:scale(2.0, 1.25, 1.25);
hips_top:translate(0.0, -2.0, 0.0);
hips_top:set_material(grey)

hips_bottom = gr.sphere('hips_bottom')
hips_top:add_child(hips_bottom)
hips_bottom:scale(1.5, 1, 1);
hips_bottom:translate(0.0, -0.75, 0.0);
hips_bottom:set_material(grey)

-- left arm
left_shoulder_joint = gr.joint('left_shoulder_joint', {-30, 0, 180}, {0.0, 0.0, 0.0});
left_shoulder_joint:translate(-2.75, 0.5, 0)
shoulders:add_child(left_shoulder_joint);

left_shoulder_pad = gr.sphere('left_shoulder_pad')
left_shoulder_joint:add_child(left_shoulder_pad)
left_shoulder_pad:scale(0.6, 1.0, 1.35);
left_shoulder_pad:rotate('z', -35);
left_shoulder_pad:set_material(purple)

left_upper_arm = gr.sphere('left_upper_arm')
left_shoulder_joint:add_child(left_upper_arm)
left_upper_arm:scale(0.5, 2.0, 1.0)
left_upper_arm:translate(0, -2.25, 0)
left_upper_arm:set_material(grey)

left_elbow_joint = gr.joint('left_elbow_joint', {0.0, 0.0, 120.0}, {0.0, 0.0, 0.0});
left_elbow_joint:translate(0, -2.0, 0)
left_upper_arm:add_child(left_elbow_joint);

left_forearm = gr.sphere('left_forearm')
left_elbow_joint:add_child(left_forearm)
left_forearm:scale(0.5, 1.5, 0.6)
left_forearm:translate(0, -1.25, 0)
left_forearm:set_material(grey)

left_brace = gr.sphere('left_brace')
left_forearm:add_child(left_brace)
left_brace:scale(0.55, 1.25, 0.70)
left_brace:translate(0, -0.25, 0)
left_brace:set_material(dirty_white)

left_exterior_brace = gr.sphere('left_exterior_brace')
left_brace:add_child(left_exterior_brace)
left_exterior_brace:scale(0.4, 0.85, 0.4)
left_exterior_brace:translate(-0.25, -0.25, 0)
left_exterior_brace:set_material(purple)

left_wrist_joint = gr.joint('left_wrist_joint', {-45.0, 0.0, 75.0}, {0.0, 0.0, 0.0});
left_wrist_joint:translate(0, -1.75, 0)
left_forearm:add_child(left_wrist_joint);

left_hand = gr.sphere('left_hand')
left_wrist_joint:add_child(left_hand)
left_hand:scale(0.6, 0.6, 0.6)
left_hand:translate(0, -0.25, 0)
left_hand:set_material(grey)

-- Right arm

right_shoulder_joint = gr.joint('right_shoulder_joint', {-30, 0, 180}, {0.0, 0.0, 0.0});
right_shoulder_joint:translate(2.75, 0.5, 0)
shoulders:add_child(right_shoulder_joint);

right_shoulder_pad = gr.sphere('right_shoulder_pad')
shoulders:add_child(right_shoulder_pad)
right_shoulder_pad:scale(0.6, 1.0, 1.35);
right_shoulder_pad:translate(2.75, 0.5, 0.0);
right_shoulder_pad:rotate('z', 35);
right_shoulder_pad:set_material(purple)

right_upper_arm = gr.sphere('right_upper_arm')
right_shoulder_joint:add_child(right_upper_arm)
right_upper_arm:scale(0.5, 2.0, 1.0)
right_upper_arm:translate(0, -2.25, 0)
right_upper_arm:set_material(grey)

right_elbow_joint = gr.joint('right_elbow_joint', {0.0, 0.0, 120.0}, {0.0, 0.0, 0.0});
right_elbow_joint:translate(0, -2.0, 0)
right_upper_arm:add_child(right_elbow_joint);

right_forearm = gr.sphere('right_forearm')
right_elbow_joint:add_child(right_forearm)
right_forearm:scale(0.5, 1.5, 0.6)
right_forearm:translate(0, -1.25, 0)
right_forearm:set_material(grey)

right_brace = gr.sphere('right_brace')
right_forearm:add_child(right_brace)
right_brace:scale(0.55, 1.25, 0.70)
right_brace:translate(0, -0.25, 0)
right_brace:set_material(dirty_white)

right_exterior_brace = gr.sphere('right_exterior_brace')
right_brace:add_child(right_exterior_brace)
right_exterior_brace:scale(0.4, 0.85, 0.4)
right_exterior_brace:translate(0.25, -0.25, 0)
right_exterior_brace:set_material(purple)

right_wrist_joint = gr.joint('right_wrist_joint', {-45.0, 0.0, 75.0}, {0.0, 0.0, 0.0});
right_wrist_joint:translate(0, -1.75, 0)
right_forearm:add_child(right_wrist_joint);

right_hand = gr.sphere('right_hand')
right_wrist_joint:add_child(right_hand)
right_hand:scale(0.6, 0.6, 0.6)
right_hand:translate(0, -0.25, 0)
right_hand:set_material(grey)

-- left leg
left_hip_joint = gr.joint('left_hip_joint', {0.0, 0.0, 110.0}, {0.0, 0.0, 0.0});
left_hip_joint:translate(-1.0, -0.5, 0)
hips_bottom:add_child(left_hip_joint);

left_thigh = gr.sphere('left_thigh')
left_hip_joint:add_child(left_thigh)
left_thigh:scale(1.1, 3.0, 1.25)
left_thigh:translate(0, -2.25, 0)
left_thigh:set_material(grey)

left_knee_joint = gr.joint('left_knee_joint', {-135.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
left_knee_joint:translate(0.0, -2.4, 0.0)
left_thigh:add_child(left_knee_joint)

left_calf = gr.sphere('left_calf')
left_knee_joint:add_child(left_calf)
left_calf:scale(.7, 2.5, .8)
left_calf:translate(0.0, -2.85, 0.0)
left_calf:set_material(grey)

left_calf_brace = gr.sphere('left_calf_brace')
left_calf:add_child(left_calf_brace)
left_calf_brace:scale(.85, 2.3, .95)
left_calf_brace:translate(0.0, -0.2, 0.0)
left_calf_brace:set_material(dirty_white)

left_calf_exterior_brace = gr.sphere('left_calf_exterior_brace')
left_calf_brace:add_child(left_calf_exterior_brace)
left_calf_exterior_brace:scale(0.6, 2.0, .7)
left_calf_exterior_brace:translate(0, -0.2, -0.3)
left_calf_exterior_brace:set_material(purple)

left_ankle_joint = gr.joint('left_ankle_joint', {-60, 0.0, 20.0}, {0.0, 0.0, 0.0});
left_ankle_joint:translate(0.0, -2.4, 0.0)
left_calf:add_child(left_ankle_joint)

left_foot = gr.sphere('left_foot')
left_ankle_joint:add_child(left_foot)
left_foot:scale(0.65, 1, 0.2)
left_foot:translate(0.0, 0.0, -1.0)
left_foot:rotate('x', 90.0);
left_foot:set_material(grey)

left_middle_toe = gr.sphere('left_middle_toe')
left_foot:add_child(left_middle_toe)
left_middle_toe:scale(0.2, 0.9, 0.25)
left_middle_toe:translate(0.0, -.5, 0)
left_middle_toe:set_material(dirty_white)

left_left_toe = gr.sphere('left_left_toe')
left_foot:add_child(left_left_toe)
left_left_toe:scale(0.2, 0.9, 0.25)
left_left_toe:translate(-0.5, -0.25, 0)
left_left_toe:rotate('z', -45)
left_left_toe:set_material(dirty_white)

left_right_toe = gr.sphere('left_right_toe')
left_foot:add_child(left_right_toe)
left_right_toe:scale(0.2, 0.9, 0.25)
left_right_toe:translate(0.5, -0.25, 0)
left_right_toe:rotate('z', 45)
left_right_toe:set_material(dirty_white)

-- right leg
right_hip_joint = gr.joint('right_hip_joint', {0.0, 0.0, 110.0}, {0.0, 0.0, 0.0});
right_hip_joint:translate(1.0, -0.5, 0)
hips_bottom:add_child(right_hip_joint);

right_thigh = gr.sphere('right_thigh')
right_hip_joint:add_child(right_thigh)
right_thigh:scale(1.1, 3.0, 1.25)
right_thigh:translate(0, -2.25, 0)
right_thigh:set_material(grey)

right_knee_joint = gr.joint('right_knee_joint', {-135.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
right_knee_joint:translate(0.0, -2.4, 0.0)
right_thigh:add_child(right_knee_joint)

right_calf = gr.sphere('right_calf')
right_knee_joint:add_child(right_calf)
right_calf:scale(.7, 2.5, .8)
right_calf:translate(0.0, -2.85, 0.0)
right_calf:set_material(grey)

right_calf_brace = gr.sphere('right_calf_brace')
right_calf:add_child(right_calf_brace)
right_calf_brace:scale(.85, 2.3, .95)
right_calf_brace:translate(0.0, -0.2, 0.0)
right_calf_brace:set_material(dirty_white)

right_calf_exterior_brace = gr.sphere('right_calf_exterior_brace')
right_calf_brace:add_child(right_calf_exterior_brace)
right_calf_exterior_brace:scale(0.6, 2.0, .7)
right_calf_exterior_brace:translate(0, -0.2, -0.3)
right_calf_exterior_brace:set_material(purple)

right_ankle_joint = gr.joint('right_ankle_joint', {-60, 0.0, 20.0}, {0.0, 0.0, 0.0});
right_ankle_joint:translate(0.0, -2.4, 0.0)
right_calf:add_child(right_ankle_joint)

right_foot = gr.sphere('right_foot')
right_ankle_joint:add_child(right_foot)
right_foot:scale(0.65, 1, 0.2)
right_foot:translate(0.0, 0.0, -1.0)
right_foot:rotate('x', 90.0);
right_foot:set_material(grey)

right_middle_toe = gr.sphere('right_middle_toe')
right_foot:add_child(right_middle_toe)
right_middle_toe:scale(0.2, 0.9, 0.25)
right_middle_toe:translate(0.0, -.5, 0)
right_middle_toe:set_material(dirty_white)

right_left_toe = gr.sphere('right_left_toe')
right_foot:add_child(right_left_toe)
right_left_toe:scale(0.2, 0.9, 0.25)
right_left_toe:translate(-0.5, -0.25, 0)
right_left_toe:rotate('z', -45)
right_left_toe:set_material(dirty_white)

right_right_toe = gr.sphere('right_right_toe')
right_foot:add_child(right_right_toe)
right_right_toe:scale(0.2, 0.9, 0.25)
right_right_toe:translate(0.5, -0.25, 0)
right_right_toe:rotate('z', 45)
right_right_toe:set_material(dirty_white)

-- tail
tail_hip = gr.sphere('tail_hip')
hips_top:add_child(tail_hip)
tail_hip:scale(1.0, 1.0, 1.5)
tail_hip:translate(0, -0.2, 1.0)
tail_hip:set_material(grey)

tail_hip_joint = gr.joint('tail_hip_joint', {-70, 0.0, 70}, {-70, 0.0, 70})
tail_hip_joint:translate(0.0, 0.0, 0.5)
tail_hip:add_child(tail_hip_joint)

tail_p1 = gr.sphere('tail_p1')
tail_hip_joint:add_child(tail_p1)
tail_p1:scale(1.0, 1.0, 2)
tail_p1:translate(0, 0.0, 2.75)
tail_p1:set_material(grey)

tail_p1_joint = gr.joint('tail_p1_joint', {-70, 0.0, 70}, {-70, 0.0, 70})
tail_p1_joint:translate(0.0, 0.0, 2)
tail_p1:add_child(tail_p1_joint)

tail_p1 = gr.sphere('tail_p1')
tail_p1_joint:add_child(tail_p1)
tail_p1:scale(0.8, 0.8, 2)
tail_p1:translate(0, 0.0, 1.5)
tail_p1:set_material(grey)

tail_p2_joint = gr.joint('tail_p2_joint', {-70, 0.0, 70}, {-70, 0.0, 70})
tail_p2_joint:translate(0.0, 0.0, 2)
tail_p1:add_child(tail_p2_joint)

tail_p2 = gr.sphere('tail_p2')
tail_p2_joint:add_child(tail_p2)
tail_p2:scale(0.6, 0.6, 1.5)
tail_p2:translate(0, 0.0, 1.35)
tail_p2:set_material(grey)

tail_p3_joint = gr.joint('tail_p3_joint', {-70, 0.0, 70}, {-70, 0.0, 70})
tail_p3_joint:translate(0.0, 0.0, 1.5)
tail_p2:add_child(tail_p3_joint)

tail_p3 = gr.sphere('tail_p3')
tail_p3_joint:add_child(tail_p3)
tail_p3:scale(0.4, 0.4, 1.0)
tail_p3:translate(0, 0.0, 0.9)
tail_p3:set_material(grey)

tail_p4_joint = gr.joint('tail_p4_joint', {-70, 0.0, 70}, {-70, 0.0, 70})
tail_p4_joint:translate(0.0, 0.0, 1)
tail_p3:add_child(tail_p4_joint)

tail_p4 = gr.sphere('tail_p4')
tail_p4_joint:add_child(tail_p4)
tail_p4:scale(0.2, 0.2, 0.5)
tail_p4:translate(0, 0.0, 0.4)
tail_p4:set_material(grey)

-- move it back and have it face me
rootnode:translate(0.0, 0.0, -30.0)
rootnode:rotate('y', 180.0);


return rootnode
