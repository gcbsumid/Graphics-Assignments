-- Puppet

rootnode = gr.node('root')

grey = gr.material( {0.3, 0.3, 0.3}, { 0.1, 0.1, 0.1}, 10)

torso = gr.sphere('torso')
rootnode:add_child(torso)
torso:set_material(grey)

rootnode:translate(0.0, 0.0, -10.0)

return rootnode