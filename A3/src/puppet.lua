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
neck = gr.sphere('neck')
shoulder:add_child(neck)
neck:scale(0.65, 1.0, 0.65)
neck:translate(0.0, 1.5, 0.0);
neck:set_material(grey);

head = gr.sphere('head')
neck:add_child(head)
head:scale(2.0, 2.0, 2.0)
head:translate(0.0, 1.75, 0.0);
head:set_material(grey);

nose = gr.sphere('nose')
head:add_child(nose)
nose:scale(0.5, 0.5, 0.5)
nose:translate(0.0, 0.0, 2.0);
nose:set_material(grey);

LeftEye = gr.sphere('LeftEye')
head:add_child(LeftEye)
LeftEye:scale(0.35, 0.35, 0.35)
LeftEye:translate(-0.75, 0.75, 1.5);
LeftEye:set_material(grey);

RightEye = gr.sphere('RightEye')
head:add_child(RightEye)
RightEye:scale(0.35, 0.35, 0.35)
RightEye:translate(0.75, 0.75, 1.5);
RightEye:set_material(grey);

-- Left Arm
LeftUpperArm = gr.sphere('LeftUpperArm')
shoulder:add_child(LeftUpperArm)
LeftUpperArm:scale(0.75, 2.5, 0.75)
LeftUpperArm:translate(3.25, -1.75, 0.0)
LeftUpperArm:set_material(grey)

LeftForearm = gr.sphere('LeftForearm')
LeftUpperArm:add_child(LeftForearm)
LeftForearm:scale(0.55, 1.5, 0.55)
LeftForearm:translate(0.0, -3.9, 0.0)
LeftForearm:set_material(grey)

LeftHand = gr.sphere('LeftHand')
LeftForearm:add_child(LeftHand)
LeftHand:scale(0.5, 0.5, 0.5)
LeftHand:translate(0.0, -1.75, 0.0)
LeftHand:set_material(grey)

-- Right ARm
RightUpperArm = gr.sphere('RightUpperArm')
shoulder:add_child(RightUpperArm)
RightUpperArm:scale(0.75, 2.5, 0.75)
RightUpperArm:translate(-3.25, -1.75, 0.0)
RightUpperArm:set_material(grey)

RightForearm = gr.sphere('RightForearm')
RightUpperArm:add_child(RightForearm)
RightForearm:scale(0.55, 1.5, 0.55)
RightForearm:translate(0.0, -3.9, 0.0)
RightForearm:set_material(grey)

RightHand = gr.sphere('RightHand')
RightForearm:add_child(RightHand)
RightHand:scale(0.5, 0.5, 0.5)
RightHand:translate(0.0, -1.75, 0.0)
RightHand:set_material(grey)


-- Left Leg 
LeftThigh = gr.sphere('LeftThigh')
hips:add_child(LeftThigh)
LeftThigh:scale(1.1, 3.5, 1.5)
LeftThigh:translate(1.75, -3.75, 0.0)
LeftThigh:set_material(grey)

LeftCalf = gr.sphere('LeftCalf')
LeftThigh:add_child(LeftCalf)
LeftCalf:scale(1.0, 3.0, 1.25)
LeftCalf:translate(0.0, -6.25, 0.0)
LeftCalf:set_material(grey)

LeftFoot = gr.sphere('LeftFoot')
LeftCalf:add_child(LeftFoot)
LeftFoot:scale(0.65, 1.5, 0.65)
LeftFoot:translate(0.0, -3.0, 1.25)
LeftFoot:rotate('x', 90.0);
LeftFoot:set_material(grey)

-- Right Leg
RightThigh = gr.sphere('RightThigh')
hips:add_child(RightThigh)
RightThigh:scale(1.1, 3.5, 1.5)
RightThigh:translate(-1.75, -3.75, 0.0)
RightThigh:set_material(grey)

RightCalf = gr.sphere('RightCalf')
RightThigh:add_child(RightCalf)
RightCalf:scale(1.0, 3.0, 1.25)
RightCalf:translate(0.0, -6.25, 0.0)
RightCalf:set_material(grey)

RightFoot = gr.sphere('RightFoot')
RightCalf:add_child(RightFoot)
RightFoot:scale(0.65, 1.5, 0.65)
RightFoot:translate(0.0, -3.0, 1.25)
RightFoot:rotate('x', 90.0);
RightFoot:set_material(grey)

rootnode:translate(0.0, 0.0, -30.0)

return rootnode