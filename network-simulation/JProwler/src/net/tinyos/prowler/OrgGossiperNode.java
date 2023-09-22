/*
 * Copyright (c) 2003, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author: Gyorgy Balogh, Gabor Pap, Miklos Maroti
 * Date last modified: 02/09/04
 */
 
package net.tinyos.prowler;

import java.awt.*;
import java.util.ArrayList;

import com.google.gson.Gson;
import organization_interface.Group;

public class OrgGossiperNode extends Mica2Node{

	/** This field is true if this mote rebroadcasted the message already. */
	boolean sent = false;

	/** This field stores the mote from which the message was first received. */
	private Node parent = null;

	public String nodeId = null;

	public String nodeType = "mote"; //or "sink"

	boolean satisfied = false;

	ArrayList<Group> myGroups = new ArrayList<>();

	public ArrayList<String> myGroupNames;

	/**
	 * This extension of the {@link Application} baseclass does everything we
	 * expect from the broadcast application, simply forwards the message once,
	 * and that is it.
	 */
	public class GossipApplication extends Application{

		/**
		 * @param node the Node on which this application runs.
		 */
		public GossipApplication(Node node) {
			super(node);
		}

		Gson gson = new Gson();


		public boolean gotOrgInfo = false;


		private boolean isMyGroup(String groupId){
			for(String gname: myGroupNames){
				if(gname.equals((groupId)))
					return true;
			}
			return false;
		}

		private boolean doIknowEverythingIneedTo(){
			return myGroupNames.size() == myGroups.size();
		}

		long last = System.currentTimeMillis();
		public void receiveMessage(Object message, Node sender ){
			long now = System.currentTimeMillis();
			if(now-last < (5000 + Math.random()*5000))
				return;
			last = now;
			if (true){
				parent = parentNode;}
				Group group = gson.fromJson((String)message, Group.class);
				OrgGossiperNode gnode = (OrgGossiperNode) this.node;
				//Retain
				if(isMyGroup(group.name)){
					myGroups.add(group);
				}else{
					for(Group cgroup: group.subGroups){
						if(isMyGroup(cgroup.name)){
							myGroups.add(cgroup);
						}
					}
				}

				satisfied = doIknowEverythingIneedTo();

				//if(!satisfied)
					sendMessage( message );

		}
		public void sendMessageDone(){
			if(nodeType.equals("sink"))
				sent = true;
		}
	}

	public OrgGossiperNode(Simulator sim, RadioModel radioModel) {
		super(sim, radioModel);
	}

	public void setAffliatedGroupNames(ArrayList<String> groups){
		myGroupNames = groups;
	}

	public void setNodeId(String id){
		nodeId = id;
	}

	/**
	 * Draws a filled circle, which is: <br>
	 *  - blue if the node is sending a message <br>
	 *  - red if the node received a corrupted message <br>
	 *  - green if the node received a message without problems <br>
	 *  - pink if the node sent a message <br>
	 *  - and black as a default<br>
	 * It also draws a line between mote and its parent, which is another mote
	 * who sent the message first to this.
	 */ 
	public void display(Display disp){
		Graphics g = disp.getGraphics();
		int      x = disp.x2ScreenX(this.x);
		int      y = disp.y2ScreenY(this.y);


		if( sending ){                    
			g.setColor( Color.blue );
		}else if( receiving ){
			if( corrupted )
				g.setColor( Color.red );
			else
				g.setColor( Color.green );                            
		}else{
			if(satisfied)
				g.setColor(Color.green);
			else if( sent )
				g.setColor( Color.pink );
			else
				g.setColor( Color.black );
		}
		g.fillOval( x-3, y-3, 10, 19 );
		if( parent != null ){
			g.setColor( Color.black );
			int x1 = disp.x2ScreenX(parent.getX());
			int y1 = disp.y2ScreenY(parent.getY());
			g.drawLine(x,y,x1,y1);
		}
	}        


}

