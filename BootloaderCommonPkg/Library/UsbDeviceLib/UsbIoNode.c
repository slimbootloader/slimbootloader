/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbDeviceDxe.h"


/**
  The SearchNode function search a memory address for record the driver allocate
  memory region and the node to the head link list.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  Buffer            The driver alocate memory address.
  @param[out] Node              The match node record of the driver aloocate
                                memory region.
  @param[out] PNode             The pervious match node record of the driver
                                aloocate memory region.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_NOT_FOUND         The memory Buffer didn't find.
**/
EFI_STATUS
SearchNode (
  IN  EFI_USBFN_IO_PROTOCOL    *This,
  IN  VOID                     *Buffer,
  OUT USB_MEM_NODE             **Node,
  OUT USB_MEM_NODE             **PNode
  )
{
  USB_XDCI_DEV_CONTEXT *UsbFuncIoDevPtr;
  USB_MEM_NODE         *NodeL;
  USB_MEM_NODE         *PNodeL;
  EFI_STATUS           Status;

  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "SearchNode - Entry\n"));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);
  NodeL = UsbFuncIoDevPtr->FirstNodePtr;
  PNodeL = NULL;
  Status = EFI_NOT_FOUND;

  while (Node != NULL) {
    if (NodeL->AllocatePtr == Buffer) {
      break;
    }

    PNodeL = NodeL;
    NodeL = NodeL->NextPtr;
  }

  if (NodeL != NULL && Node != NULL) {
    *Node = NodeL;
    *PNode = PNodeL;
    Status = EFI_SUCCESS;
  }

  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "SearchNode - Exit %r\n", Status));
  return Status;
}

/**
  The InsertNewNodeToHead function remove a memory for record the driver allocate
  memory region and the node to the head link list.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[in]  Buffer            The driver alocate memory address.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_NOT_FOUND         The memory Buffer didn't find.
**/
EFI_STATUS
RemoveNode (
  IN  EFI_USBFN_IO_PROTOCOL    *This,
  IN  VOID                     *Buffer
  )
{
  USB_XDCI_DEV_CONTEXT *UsbFuncIoDevPtr;
  USB_MEM_NODE         *Node;
  USB_MEM_NODE         *PNode;
  EFI_STATUS           Status;

  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "RemoveNode - Entry\n"));

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  Status = SearchNode (This, Buffer, &Node, &PNode);

  if (EFI_ERROR (Status) || PNode == NULL) {
    DEBUG ((USB_DEBUG_MEM_NODE_ERROR, "RemoveNode - Node Not Found\n"));
    return EFI_NOT_FOUND;
  }

  if (Node != UsbFuncIoDevPtr->FirstNodePtr) {
    PNode->NextPtr = Node->NextPtr;
  } else {
    UsbFuncIoDevPtr->FirstNodePtr = Node->NextPtr;
  }

  FreePool (Node->AllocatePtr);
  FreePool (Node);
  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "RemoveNode - Exit\n"));
  return EFI_SUCCESS;
}

/**
  The InsertNewNodeToHead function allocates a memory for record the driver allocate
  memory region and insert the node to the head link list.

  @param[in]  This              A pointer to the EFI_USBFN_IO_PROTOCOL instance.
  @param[out] Node              return the new node address.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES  The requested transfer Buffer could not be allocated.

**/
EFI_STATUS
InsertNewNodeToHead (
  IN  EFI_USBFN_IO_PROTOCOL    *This,
  OUT USB_MEM_NODE             **Node
  )
{
  USB_MEM_NODE         *NewNodePtr;
  USB_MEM_NODE         *CurrentNodePtr;
  USB_XDCI_DEV_CONTEXT *UsbFuncIoDevPtr;
  EFI_STATUS           Status;

  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "CreateNewNode - Entry\n"));

  if (This == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  UsbFuncIoDevPtr = USBFUIO_CONTEXT_FROM_PROTOCOL (This);

  //
  // Create the new node
  //
  NewNodePtr = AllocateZeroPool (sizeof (USB_MEM_NODE));
  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "NewNodePtr - Addr = 0x%08x\n", (UINTN)NewNodePtr));

  if (NewNodePtr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  //
  // insert the new node
  //
  CurrentNodePtr = UsbFuncIoDevPtr->FirstNodePtr;
  UsbFuncIoDevPtr->FirstNodePtr = NewNodePtr;

  if (CurrentNodePtr != NULL) {
    NewNodePtr->NextPtr = CurrentNodePtr;
  }

  *Node = NewNodePtr;

  DEBUG ((USB_DEBUG_MEM_NODE_INFO, "CreateNewNode - Exit\n"));
  return EFI_SUCCESS;

ErrorExit:

  DEBUG ((USB_DEBUG_MEM_NODE_ERROR, "CreateNewNode - error %r\n", Status));
  return Status;
}

