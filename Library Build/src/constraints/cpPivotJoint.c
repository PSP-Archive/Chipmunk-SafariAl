/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>

#include "../chipmunk.h"
#include "util.h"

static void
preStep(cpPivotJoint *joint, cpFloat dt, cpFloat dt_inv)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	joint->r1 = cpvrotate(joint->anchr1, a->rot);
	joint->r2 = cpvrotate(joint->anchr2, b->rot);
	
	// Calculate mass tensor
	k_tensor(a, b, joint->r1, joint->r2, &joint->k1, &joint->k2);
	
	// compute max impulse
	joint->jMaxLen = J_MAX(joint, dt);
	
	// calculate bias velocity
	cpVect delta = cpvsub(cpvadd(b->p, joint->r2), cpvadd(a->p, joint->r1));
	joint->bias = clamp_vect(cpvmult(delta, -joint->constraint.biasCoef*dt_inv), joint->constraint.maxBias);
	
	// apply accumulated impulse
	apply_impulses(a, b, joint->r1, joint->r2, joint->jAcc);
}

static void
applyImpulse(cpPivotJoint *joint)
{
	cpBody *a = joint->constraint.a;
	cpBody *b = joint->constraint.b;
	
	cpVect r1 = joint->r1;
	cpVect r2 = joint->r2;
		
	// compute relative velocity
	cpVect vr = relative_velocity(a, b, r1, r2);
	
	// compute normal impulse
	cpVect j = mult_k(cpvsub(joint->bias, vr), joint->k1, joint->k2);
	cpVect jOld = joint->jAcc;
	joint->jAcc = clamp_vect(cpvadd(joint->jAcc, j), joint->jMaxLen);
	j = cpvsub(joint->jAcc, jOld);
	
	// apply impulse
	apply_impulses(a, b, joint->r1, joint->r2, j);
}

static cpFloat
getImpulse(cpConstraint *joint)
{
	return cpvlength(((cpPivotJoint *)joint)->jAcc);
}

const cpConstraintClass cpPivotJointClass = {
	(cpConstraintPreStepFunction)preStep,
	(cpConstraintApplyImpulseFunction)applyImpulse,
	(cpConstraintGetImpulseFunction)getImpulse,
};

cpPivotJoint *
cpPivotJointAlloc(void)
{
	return (cpPivotJoint *)malloc(sizeof(cpPivotJoint));
}

cpPivotJoint *
cpPivotJointInit(cpPivotJoint *joint, cpBody *a, cpBody *b, cpVect anchr1, cpVect anchr2)
{
	cpConstraintInit((cpConstraint *)joint, &cpPivotJointClass, a, b);
	
//	joint->anchr1 = cpvunrotate(cpvsub(pivot, a->p), a->rot);
//	joint->anchr2 = cpvunrotate(cpvsub(pivot, b->p), b->rot);
	joint->anchr1 = anchr1;
	joint->anchr2 = anchr2;
	
	joint->jAcc = cpvzero;
	
	return joint;
}

cpConstraint *
cpPivotJointNew(cpBody *a, cpBody *b, cpVect anchr1, cpVect anchr2)
{
	return (cpConstraint *)cpPivotJointInit(cpPivotJointAlloc(), a, b, anchr1, anchr2);
}
