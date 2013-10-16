-- Puppet

rootnode = gr.node('root')

grey = gr.material( {0.3, 0.3, 0.3}, { 0.1, 0.1, 0.1}, 10)

torso = gr.sphere('torso')
rootnode:add_child(torso)
torso:scale(2.5, 4.0, 1.5);
torso:set_material(grey)

shoulder = gr.sphere('shoulder')
torso:add_child(shoulder)
shoulder:scale(3.5, 1.0, 1.5)
shoulder:translate(0.0, 3.0, 0.0)
shoulder:set_material(grey)

hips = gr.sphere('hips')
torso:add_child(hips)
hips:scale(2.65, 1.0, 1.5)
hips:translate(0.0, -3.75, 0.0)
hips:set_material(grey)

-- Head
neck_joint = gr.joint('neck_joint', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
shoulder:add_child(neck_joint)

neck = gr.sphere('neck')
neck_joint:add_child(neck)
neck:scale(0.65, 1.0, 0.65)
neck:translate(0.0, 1.5, 0.0);
neck:set_material(grey);

head_joint = gr.joint('head_joint_x', {-45.0, 0.0, 45.0}, {-80.0, 0.0, 80.0})
neck:add_child(head_joint);

head = gr.sphere('head')
head_joint:add_child(head)
head:scale(2.0, 2.0, 2.0)
head:translate(0.0, 1.75, 0.0);
head:set_material(grey);

nose = gr.sphere('nose')
head:add_child(nose)
nose:scale(0.5, 0.5, 0.5)
nose:translate(0.0, 0.0, -2.0);
nose:set_material(grey);

LeftEye = gr.sphere('LeftEye')
head:add_child(LeftEye)
LeftEye:scale(0.35, 0.35, 0.35)
LeftEye:translate(-0.75, 0.75, -1.5);
LeftEye:set_material(grey);

RightEye = gr.sphere('RightEye')
head:add_child(RightEye)
RightEye:scale(0.35, 0.35, 0.35)
RightEye:translate(0.75, 0.75, -1.5);
RightEye:set_material(grey);

-- Left Arm
LeftShoulderJoint = gr.joint('LeftShoulderJoint', {-30.0, 0.0, 180.0}, {0.0, 0.0, 0.0});
LeftShoulderJoint:translate(3.25, 0.0, 0.0)
shoulder:add_child(LeftShoulderJoint)

LeftUpperArm = gr.sphere('LeftUpperArm')
LeftShoulderJoint:add_child(LeftUpperArm)
LeftUpperArm:scale(0.75, 2.5, 0.75)
LeftUpperArm:translate(0.0, -1.75, 0.0)
LeftUpperArm:set_material(grey)

LeftElbowJoint = gr.joint('LeftElbowJoint', {0.0, 0.0, 120.0}, {0.0, 0.0, 0.0});
LeftElbowJoint:translate(0.0, -2.3, 0.0)
LeftUpperArm:add_child(LeftElbowJoint)

LeftForearm = gr.sphere('LeftForearm')
LeftElbowJoint:add_child(LeftForearm)
LeftForearm:scale(0.55, 1.5, 0.55)
LeftForearm:translate(0.0, -1.5, 0.0)
LeftForearm:set_material(grey)

LeftWristJoint = gr.joint('LeftWristJoint', {-25.0, 0.0, 45.0}, {0.0, 0.0, 0.0});
LeftWristJoint:translate(0.0, -1.5, 0.0)
LeftForearm:add_child(LeftWristJoint)

LeftHand = gr.sphere('LeftHand')
LeftWristJoint:add_child(LeftHand)
LeftHand:scale(0.5, 0.5, 0.5)
LeftHand:translate(0.0, -0.35, 0.0)
LeftHand:set_material(grey)

-- Right Arm
RightShoulderJoint = gr.joint('RightShoulderJoint', {-30.0, 0.0, 180.0}, {0.0, 0.0, 0.0});
RightShoulderJoint:translate(-3.25, 0.0, 0.0)
shoulder:add_child(RightShoulderJoint)

RightUpperArm = gr.sphere('RightUpperArm')
RightShoulderJoint:add_child(RightUpperArm)
RightUpperArm:scale(0.75, 2.5, 0.75)
RightUpperArm:translate(0.0, -1.75, 0.0)
RightUpperArm:set_material(grey)

RightElbowJoint = gr.joint('RightElbowJoint', {0.0, 0.0, 120.0}, {0.0, 0.0, 0.0});
RightElbowJoint:translate(0.0, -2.3, 0.0)
RightUpperArm:add_child(RightElbowJoint)

RightForearm = gr.sphere('RightForearm')
RightElbowJoint:add_child(RightForearm)
RightForearm:scale(0.55, 1.5, 0.55)
RightForearm:translate(0.0, -1.5, 0.0)
RightForearm:set_material(grey)

RightWristJoint = gr.joint('RightWristJoint', {-25.0, 0.0, 45.0}, {0.0, 0.0, 0.0});
RightWristJoint:translate(0.0, -1.5, 0.0)
RightForearm:add_child(RightWristJoint)

RightHand = gr.sphere('RightHand')
RightWristJoint:add_child(RightHand)
RightHand:scale(0.5, 0.5, 0.5)
RightHand:translate(0.0, -0.35, 0.0)
RightHand:set_material(grey)


-- Left Leg 
LeftHipJoint = gr.joint('LeftHipJoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0});
LeftHipJoint:translate(1.75, 0.0, 0.0)
hips:add_child(LeftHipJoint)

LeftThigh = gr.sphere('LeftThigh')
LeftHipJoint:add_child(LeftThigh)
LeftThigh:scale(1.1, 3.5, 1.5)
LeftThigh:translate(0.0, -3.75, 0.0)
LeftThigh:set_material(grey)

LeftKneeJoint = gr.joint('LeftKneeJoint', {-135.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
LeftKneeJoint:translate(0.0, -3.4, 0.0)
LeftThigh:add_child(LeftKneeJoint)

LeftCalf = gr.sphere('LeftCalf')
LeftKneeJoint:add_child(LeftCalf)
LeftCalf:scale(0.9, 3.0, 1.25)
LeftCalf:translate(0.0, -2.85, 0.0)
LeftCalf:set_material(grey)

LeftAnkleJoint = gr.joint('LeftAnkleJoint', {-60, 0.0, 20.0}, {0.0, 0.0, 0.0});
LeftAnkleJoint:translate(0.0, -3.0, 0.0)
LeftCalf:add_child(LeftAnkleJoint)

LeftFoot = gr.sphere('LeftFoot')
LeftAnkleJoint:add_child(LeftFoot)
LeftFoot:scale(0.65, 1.5, 0.65)
LeftFoot:translate(0.0, 0.0, -1.25)
LeftFoot:rotate('x', 90.0);
LeftFoot:set_material(grey)

-- Right Leg
RightHipJoint = gr.joint('RightHipJoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0});
RightHipJoint:translate(-1.75, 0.0, 0.0)
hips:add_child(RightHipJoint)

RightThigh = gr.sphere('RightThigh')
RightHipJoint:add_child(RightThigh)
RightThigh:scale(1.1, 3.5, 1.5)
RightThigh:translate(0.0, -3.75, 0.0)
RightThigh:set_material(grey)

RightKneeJoint = gr.joint('RightKneeJoint', {-135.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
RightKneeJoint:translate(0.0, -3.4, 0.0)
RightThigh:add_child(RightKneeJoint)

RightCalf = gr.sphere('RightCalf')
RightKneeJoint:add_child(RightCalf)
RightCalf:scale(0.9, 3.0, 1.25)
RightCalf:translate(0.0, -2.85, 0.0)
RightCalf:set_material(grey)

RightAnkleJoint = gr.joint('RightAnkleJoint', {-60, 0.0, 20.0}, {0.0, 0.0, 0.0});
RightAnkleJoint:translate(0.0, -3.0, 0.0)
RightCalf:add_child(RightAnkleJoint)

RightFoot = gr.sphere('RightFoot')
RightAnkleJoint:add_child(RightFoot)
RightFoot:scale(0.65, 1.5, 0.65)
RightFoot:translate(0.0, 0.0, -1.25)
RightFoot:rotate('x', 90.0);
RightFoot:set_material(grey)

rootnode:translate(0.0, 0.0, -30.0)
rootnode:rotate('y', 180.0);

return rootnode