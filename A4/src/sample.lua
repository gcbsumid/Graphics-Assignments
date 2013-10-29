-- A simple scene with five spheres

stone = gr.refl_material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 25, 0.0)
mat2 = gr.refl_material({0.0, 0.2, 0.5}, {0.5, 0.7, 0.5}, 50, 0.0)
mat3 = gr.refl_material({1.0, 0.2, 0.1}, {0.5, 0.7, 0.5}, 50, 0.0)
mat4 = gr.refl_material({0.0, 0.6, 0.6}, {0.5, 0.7, 0.5}, 50, 1.0)
blue_refl = gr.refl_material({0.0, 0.2, 0.6}, {0.5, 0.7, 0.5}, 50, 0.5)

scene = gr.node('root')
scene:rotate('X', 23);
scene:translate(0, 0, 5)

ring = gr.cube('s1');
scene:add_child(ring);
ring:set_refl_material(mat3);
ring:translate(0.0, 0.0, -10);
ring:scale(13, 2, 13);

mirror = gr.sphere('mirror') 
scene:add_child(mirror);
mirror:set_refl_material(mat4);
mirror:translate(0.0, 6.0, -10);
mirror:scale(3, 3, 3);

bump1 = gr.sphere('bump1') 
scene:add_child(bump1);
bump1:set_refl_material(blue_refl);
bump1:translate(7, 2.5, -5);
bump1:scale(3, 2, 2);

bump2 = gr.sphere('bump2') 
scene:add_child(bump2);
bump2:set_refl_material(blue_refl);
bump2:translate(-7, 2.5, -5);
bump2:scale(3, 2, 2);

corner1 = gr.cube('corner1') 
scene:add_child(corner1);
corner1:set_refl_material(stone);
corner1:translate(10, 4.0, -22.5);
corner1:scale(1, 4, 1);

corner2 = gr.cube('corner2') 
scene:add_child(corner2);
corner2:set_refl_material(stone);
corner2:translate(-10, 4.0, -22.5);
corner2:scale(1, 4, 1);

corner3 = gr.cube('corner3') 
scene:add_child(corner3);
corner3:set_refl_material(stone);
corner3:translate(10, 4.0, 2.5);
corner3:scale(1, 4, 1);

corner4 = gr.cube('corner4') 
scene:add_child(corner4);
corner4:set_refl_material(stone);
corner4:translate(-10, 4.0, 2.5);
corner4:scale(1, 4, 1);

require('mickey')
mickey:set_refl_material(mat2)

mickey1 = gr.node('mickey1') 
scene:add_child(mickey1)
mickey1:add_child(mickey)
-- mickey1:translate(0.0, 6.0, -10);
mickey1:rotate('y', 90);
mickey1:rotate('x', -90);
mickey1:translate(0, 2, -10);
mickey1:scale(15, 15, 15)

mickey2 = gr.node('mickey2') 
scene:add_child(mickey2)
mickey2:add_child(mickey)
-- mickey2:translate(0.0, 6.0, -10);
mickey2:rotate('y', 90);
mickey2:rotate('x', -90);
mickey2:translate(15, 2, -10);
mickey2:scale(15, 15, 15)

-- s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
-- scene:add_child(s1)
-- s1:set_refl_material(mat1)

-- s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
-- scene:add_child(s2)
-- s2:set_refl_material(mat1)

-- s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
-- scene:add_child(s3)
-- s3:set_refl_material(mat4)

-- s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
-- scene:add_child(s4)
-- s4:set_refl_material(mat3)

-- -- s4 = gr.nh_box('s4', {25, 25, -50}, 50)
-- -- scene:add_child(s4)
-- -- s4:set_refl_material(mat4)

-- s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
-- scene:add_child(s5)
-- s5:set_refl_material(mat1)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {1.0, 0.4, 0.0}, {1, 0, 0})

gr.render(scene, 'sample.png', 500, 500,
    {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 90,
    -- {0.3, 0.3, 0.3}, {orange_light})
    {0.3, 0.3, 0.3}, {white_light, orange_light})
