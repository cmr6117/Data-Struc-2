I was generally able to create the Octree but for some reason I was unable to get it to recognize entities that were within it, which, frustratingly enough, broke the whole thing. I spent a long time trying to figure out what was causing this issue, whether it had something to do with the MyEntityManager class or the MyOctree class or something else but I was unable to figure it out by the end. 

The m_uOctantLevels variable that dictates the level the Octree can reach is taken into consideration by the class.
The Octree is regenerated regularly with + and - changing it immediatly. 
The Octree root still changes what is displayed with PgUp and PgDown. Collisions still highlight. 
Were the Octree able to recognize the Entities it would definitely cause a significant increase in the Framerate since the entities would only need to check others within their dimension.